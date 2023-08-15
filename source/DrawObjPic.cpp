
#include "StdAfx.h"
#include "DrawObjPic.h"
#include "CvvImage.h"
#include "Resource.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

DrawObjPic g_DrawObjPic;
CObjectAnnoDlg *m_pObjectAnnoDlg = NULL;

DrawObjPic::DrawObjPic(void)
:m_bOnLButtonUp(false)
,m_bOnLabelLBtnDown(false)
,m_bExitThread(false)
,m_bRunNextFile(false)
,m_fWidthScale(1.f)
,m_fHeightScale(1.f)
,m_fRectWidthScale(1.f)
,m_fRectHeightScale(1.f)
,m_nFileCnt(0)
,m_nFileIndex(0)
,m_nObjAnnoNum(0)
,m_OriImageWidth(0)
,m_OriImageHeight(0)
,FileType(1)
,m_bPause(true)
,m_bPlayRect(false)
,m_bPlayRectWnd(false)
,m_nCtrlPlaySpeed(200)
,m_bPlayForward(false)
,m_bPlayBackward(false)
,m_nVideoFrameNum(0)
,m_nVideoFrameIdx(0)
,m_bObjRectDrawed(false)
,m_bObjInfoConfirmed(false)
,m_bModifyObjRect(false)
,m_bModifyObjLabel(false)
,m_bLabelBGArea(false)
,m_bLabelOcclution(false)
,m_bSliderChanged(false)
,m_iObjectSel(-1)
,m_iObjectAngleX(0)
,m_iObjectAngleY(0)
,m_bObjectInfoSaved(false)
{
	m_hObjThread     = NULL;
    m_hCaptureThread = NULL;
	m_hZoomWnd       = NULL;
    m_ImageShow      = NULL;

    m_ObjPos         = cvPoint(0,0);
    m_ObjCurPos      = cvPoint(0,0);
    m_CurCursorPos.x = 0;
    m_CurCursorPos.y = 0;

    m_PlayPointTL    = cvPoint(0, 0);   //播放区域坐标
    m_PlayPointBR    = cvPoint(0, 0);


    memset(m_bObjPtSelect, false, 4 * sizeof(false));
    memset(m_aExePath, '\0', _MAX_PATH);
	memset(m_aFilePath, '\0', _MAX_PATH);
    memset(m_iObjectLabelMap, -1, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));

    ClearObjectInfo();

    m_pImageZoomShow = cvCreateImage(cvSize(Image_Zoom_Width, Image_Zoom_Height), IPL_DEPTH_8U, 3);
	//m_pObjZoomShow = cvCreateImage(cvSize(Image_Zoom_Width * 1.5f, Image_Zoom_Height * 1.5f), IPL_DEPTH_8U, 3);

	InitializeCriticalSection(&m_PlayCriticalSection);
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

DrawObjPic::~DrawObjPic(void)
{
	if(m_ImageShow != NULL)
	{
		delete m_ImageShow;
		m_ImageShow = NULL;
	}

	if(m_hZoomWnd != NULL)
	{
		cvDestroyWindow("Zoom-Image");
		m_hZoomWnd = NULL;
	}

    if (m_pImageZoomShow != NULL)
    {
        cvReleaseImage(&m_pImageZoomShow);
        m_pImageZoomShow = NULL;
    }

	DeleteCriticalSection(&m_PlayCriticalSection);
	::CoUninitialize();
}

bool DrawObjPic::UpdateImageRect() //初始化
{
    if (!m_pObjectAnnoDlg)
    {
        m_pObjectAnnoDlg = (CObjectAnnoDlg*)m_pObjAnnoWnd;
    }

	RECT WindowRect;
	m_pObjectAnnoDlg->m_Static_ImageShow.GetWindowRect(&WindowRect);
    m_pObjectAnnoDlg->m_Static_ImageShow.GetClientRect(&m_ImageRect);

    POINT lPoint = {WindowRect.left, WindowRect.top};
	ScreenToClient(m_hWnd, &lPoint);
	m_OffSetToWindowRectX = lPoint.x;
	m_OffSetToWindowRectY = lPoint.y;

	return true;
}

bool DrawObjPic::LoadImageScource(UINT flags)
{
	FileType = flags;

	CString strFileDir, strFileExt, strFileName;
	if(FileType == 1)  //导入的是图片
	{
        CFileDialog dlg(true, NULL, NULL, OFN_HIDEREADONLY, _T("jpeg (*.jpg)|*.jpg|BMP (*.bmp)|*.bmp|png (*.png)|*.png|所有文件 (*.*)|*.*||"), NULL, 0);

		if(dlg.DoModal() == IDOK)
		{
            strFileDir  = dlg.GetPathName();
            strFileName = dlg.GetFileName();
            strFileExt  = dlg.GetFileExt();

            int pos = strFileDir.ReverseFind('\\');

            strFileDir.Delete(pos + 1, strFileName.GetLength()); 
		}
		else
		{
			//AfxMessageBox("图片加载失败");
			return false;
		}  
	}
	else
	{
		CFileDialog dlg(true, NULL, NULL, OFN_HIDEREADONLY, _T("AVI文件 (*.avi)|*.avi|所有文件 (*.*)|*.*||"), NULL, 0);

		if(dlg.DoModal() == IDOK)
		{
            strFileDir  = dlg.GetPathName();
            strFileName = dlg.GetFileName();
            strFileExt  = dlg.GetFileExt();

            int pos = strFileDir.ReverseFind('\\');

            strFileDir.Delete(pos + 1, strFileName.GetLength()); 
		}
		else
		{
			//AfxMessageBox("视频加载失败");
			return false;
		}
	}

	memset(m_aFilePath, 0, _MAX_PATH);
	memcpy(m_aFilePath, strFileDir.GetBuffer(0), strFileDir.GetLength());

	if(!m_vFileName.empty())
	{
		m_vFileName.clear();
		m_nFileCnt = 0;
	}

	if(!m_vObjInfo.empty())
	{
		m_vObjInfo.clear();
		m_nObjAnnoNum = 0;
	}

	CFileFind finder;
	BOOL bHave = finder.FindFile(strFileDir + CString("\\*") + strFileExt);
	while(bHave)
	{
		bHave = finder.FindNextFile( );
		m_vFileName.push_back(finder.GetFileName());
		m_nFileCnt++;
	}

	for(int i = 0; i < m_nFileCnt; i++)
	{
		if(m_vFileName[i] == strFileName)
		{
			m_nFileIndex = i;
		}
	}

    ReCreateThread();

	return true;
}

DWORD WINAPI  DrawObjPic::EngineThread(LPVOID pParam)
{
	ASSERT(pParam != NULL);
	DrawObjPic *pThis       = (DrawObjPic *)pParam;
	DWORD dwRet = 0;

	dwRet = pThis->ThreadFunc();
	return dwRet;
}


DWORD WINAPI DrawObjPic::ObjThreadProc(LPVOID pParam)
{
	ASSERT(pParam != NULL);
	DrawObjPic *pThis = (DrawObjPic *)pParam;
	DWORD dwRet = 0;
	dwRet = pThis->ObjThreadProcFunc();

	return dwRet;
}

DWORD DrawObjPic::ThreadFunc()
{
    std::string strFilePath = std::string(m_aFilePath) + m_vFileName[m_nFileIndex].GetBuffer(0); 

	if(FileType == 1)
	{
        m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetRange(1, m_vFileName.size());
        m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetPos(m_nFileIndex + 1);
        
        CString strBuf;
        strBuf.Format("当前图片进度: %d / %d", m_nFileIndex + 1, m_vFileName.size());
        m_pObjectAnnoDlg->m_Static_FileProcess.SetWindowText(strBuf);

		ReadImageFile((char*)strFilePath.c_str());
	}
	else
	{
        EnterCriticalSection(&m_PlayCriticalSection);

        strcpy(m_strCurFilePath, strFilePath.c_str());

        Sleep(10);

        LeaveCriticalSection(&m_PlayCriticalSection);

		ReadVideoFile();
	}

	return 1;
}

DWORD DrawObjPic::ObjThreadProcFunc()
{
	while(!m_bExitThread)
	{
        DrawObjectRect();
	}

	return 1;
}

void DrawObjPic::DrawOnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint Pt = CPoint(point.x - m_OffSetToWindowRectX, point.y - m_OffSetToWindowRectY);

	if(PtInRect(&m_ImageRect, Pt) && (m_bPause|| m_bPlayRect))
	{
		EnterCriticalSection(&m_PlayCriticalSection);

		m_ObjPos    = cvPoint(Pt.x, Pt.y);
		m_ObjCurPos = cvPoint(Pt.x, Pt.y);

        m_bOnLButtonUp = false;
        if (m_bLabelBGArea || m_bLabelOcclution)
        {
            m_bOnLabelLBtnDown = true;
        }

        if (m_bModifyObjRect)
        {
            for (int i = 0; i < 4; i++)
            {
                if (abs(m_CurCursorPos.x - m_ObjModifyPt[i].x) < 10 && abs(m_CurCursorPos.y - m_ObjModifyPt[i].y) < 10)
                {
                    m_bObjPtSelect[i] = true;
                    m_bObjRectDrawed = false;
                }
            }
        }

		LeaveCriticalSection(&m_PlayCriticalSection);
	}

	return;
}

void DrawObjPic::DrawOnLButtonUp(UINT nFlags, CPoint point)
{
    CPoint Pt = CPoint(point.x - m_OffSetToWindowRectX, point.y - m_OffSetToWindowRectY);

	if(PtInRect(&m_ImageRect, Pt) && (m_bPause|| m_bPlayRect))
	{
        EnterCriticalSection(&m_PlayCriticalSection);

        if (m_bLabelBGArea || m_bLabelOcclution)
        {
            m_bOnLabelLBtnDown = false;
        }

        m_bOnLButtonUp = true;

		if((m_ObjCurPos.x > m_ObjPos.x) && (m_ObjCurPos.y > m_ObjPos.y))
		{
			if(m_bPlayRect && !m_bPause && !m_bPlayRectWnd)
			{
				m_PlayPointTL = cvPoint(m_ObjPos.x, m_ObjPos.y);
				m_PlayPointBR = cvPoint(m_ObjCurPos.x, m_ObjCurPos.y);
				m_ObjPos = cvPoint(0, 0);
				m_ObjCurPos = cvPoint(0, 0);
				m_bPlayRectWnd = true;
				m_bOnLButtonUp = false;
			}
		}

        if (m_bModifyObjRect)
        {
            m_bObjRectDrawed = true;
        }

        LeaveCriticalSection(&m_PlayCriticalSection);
	}
}

void DrawObjPic::DrawOnMouseMove(UINT nFlags, CPoint point)
{
    CPoint Pt = CPoint(point.x - m_OffSetToWindowRectX, point.y - m_OffSetToWindowRectY);

    m_CurCursorPos = cvPoint(Pt.x, Pt.y);

	if(PtInRect(&m_ImageRect, Pt) && (m_bPause|| m_bPlayRect))
	{
		HCURSOR hCur = LoadCursor(NULL, IDC_CROSS);
		::SetCursor(hCur);

		if(m_bPause)
		{
		    ShowRectZoomImage();
		}

		//if( -1 == m_iObjectSel)
		{
			m_ObjCurPos = cvPoint(Pt.x, Pt.y);
		}
	}
}

void DrawObjPic::DrawObjectRect()  
{
    if (m_ImageShow == NULL)
    {
        return;
    }

    CPoint pt = CPoint(m_CurCursorPos.x, m_CurCursorPos.y); 

    int iRectWidth = (m_ImageRect.Width() / 4) * 4;
    int iRectheight = m_ImageRect.Height();

	IplImage *img = cvCreateImage(cvSize(iRectWidth, iRectheight), IPL_DEPTH_8U, 3);

	EnterCriticalSection(&m_PlayCriticalSection);

	memcpy(img->imageData, (char *)m_ImageShow, iRectWidth * iRectheight * 3 * sizeof(char));

    LeaveCriticalSection(&m_PlayCriticalSection);

    //////////////////////////////////////////////////////////////////////////
    //画鼠标十字线  要以更好地帮助画目标框
    if (PtInRect(&m_ImageRect, pt))
    {
        cvLine(img, cvPoint(m_CurCursorPos.x, 0), cvPoint(m_CurCursorPos.x, iRectheight), ORANGE, 1, 8, 0);
        cvLine(img, cvPoint(0, m_CurCursorPos.y), cvPoint(iRectWidth, m_CurCursorPos.y), ORANGE, 1, 8, 0);
    }
	
	if (m_ObjPos.x > 0 && m_ObjPos.y > 0 && m_ObjCurPos.x > 0 && m_ObjCurPos.y > 0 && -1 == m_iObjectSel)
	{
        //////////////////////////////////////////////////////////////////////////
        //拖动修改目标标注框的大小及位置
        if (m_bModifyObjRect && m_iDrawStyle == 1)
        {
            CvPoint Pt[4];
            memcpy(Pt, m_ObjModifyPt, 4 * sizeof(CvPoint));
            
            for (int i = 0; i < 4; i++)
            {
                if (m_bObjPtSelect[i])
                {
                    int index = -1;
                    int iDx = m_ObjCurPos.x - m_ObjPos.x;
                    int iDy = m_ObjCurPos.y - m_ObjPos.y;
                    Pt[i].x += iDx; Pt[i].y += iDy;

                    if (i == 0 || i == 1)
                    {
                        index = i + 2;
                    }
                    else if (i == 2 || i == 3)
                    {
                        index = i - 2;
                    }

                    Pt[index].x -= iDx;
                    Pt[index].y -= iDy;

                    if (m_bOnLButtonUp)
                    {
                        m_ObjModifyPt[i].x += iDx;
                        m_ObjModifyPt[i].y += iDy;
                        m_ObjModifyPt[index].x -= iDx;
                        m_ObjModifyPt[index].y -= iDy;
                        m_bObjPtSelect[i] = false;
                    }
                }
            } 

            for (int i = 0; i < 3; i++)
            {
                cvLine(img, Pt[i], Pt[i + 1], RED, 1, 8, 0);
                if (i == 0)
                {
                    cvLine(img, Pt[0], Pt[3], RED, 1, 8, 0);
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        //画目标标注框及栅格  或者是播放区域
		if((m_ObjCurPos.x > m_ObjPos.x) && (m_ObjCurPos.y > m_ObjPos.y) && !m_bModifyObjRect && !m_bObjRectDrawed)
		{
            if (!m_bOnLButtonUp)
            {
                if (!m_bPlayRectWnd)
                {
                    cvRectangle(img, m_ObjPos, m_ObjCurPos, GREEN, 1, 8, 0);
                }

                if (m_iDrawStyle == 0)
                {
                    int iStepX = (m_ObjCurPos.x - m_ObjPos.x) / OBJECT_BLOCK_X_NUM;
                    int iStepY = (m_ObjCurPos.y - m_ObjPos.y) / OBJECT_BLOCK_Y_NUM;

                    m_ObjCurPos.x = m_ObjPos.x + iStepX * OBJECT_BLOCK_X_NUM;
                    m_ObjCurPos.y = m_ObjPos.y + iStepY * OBJECT_BLOCK_Y_NUM;

                    if (iStepX != 0 && iStepY != 0 && m_bPause)
                    {
                        for (int x = m_ObjPos.x; x <= m_ObjCurPos.x; x += iStepX)
                        {
                            cvLine(img, cvPoint(x, m_ObjPos.y), cvPoint(x, m_ObjCurPos.y), GREEN, 1, 8, 0);
                        }

                        for (int y = m_ObjPos.y; y <= m_ObjCurPos.y; y += iStepY)
                        {
                            cvLine(img, cvPoint(m_ObjPos.x, y), cvPoint(m_ObjCurPos.x, y), GREEN, 1, 8, 0);
                        }
                    }
                }
            }
            else
            {
                m_ObjLTPt = m_ObjPos;
                m_ObjRBPt = m_ObjCurPos;

                if (m_iDrawStyle == 0)
                {
                    m_iStepX = (m_ObjRBPt.x - m_ObjLTPt.x) / OBJECT_BLOCK_X_NUM;
                    m_iStepY = (m_ObjRBPt.y - m_ObjLTPt.y) / OBJECT_BLOCK_Y_NUM;

                    if (m_iStepX == 0 || m_iStepY == 0)
                    {
                        return;
                    }

                    m_ObjRBPt.x = m_ObjLTPt.x + m_iStepX * OBJECT_BLOCK_X_NUM;
                    m_ObjRBPt.y = m_ObjLTPt.y + m_iStepY * OBJECT_BLOCK_Y_NUM;
                }
            
                m_ObjModifyPt[0] = m_ObjLTPt;
                m_ObjModifyPt[1] = cvPoint(m_ObjRBPt.x, m_ObjLTPt.y);
                m_ObjModifyPt[2] = m_ObjRBPt;
                m_ObjModifyPt[3] = cvPoint(m_ObjLTPt.x, m_ObjRBPt.y);

                m_bOnLButtonUp = false;
                m_bObjRectDrawed = true;
            }
		}
	}

    if (m_bObjRectDrawed && m_bPause)
    {
        DrawObjectRectOrGrid(img);

        if (m_bObjInfoConfirmed)
        {
            CString strBuf;
            stObjectInfo.ObjectColor = (enObjectColor)m_pObjectAnnoDlg->m_ComboCtrl_ObjectColor.GetCurSel();
            m_pObjectAnnoDlg->m_Edit_ObjectContext.GetWindowText(strBuf);

            if (!strBuf.IsEmpty())
            {
                stObjectInfo.bHaveContext = TRUE;
                strcpy(stObjectInfo.aObjectContext, strBuf.GetBuffer());
            }

			stObjectInfo.iDrawType = m_iDrawStyle;
            
            stObjectInfo.iObjectIndex = m_pObjectAnnoDlg->m_ComboCtrl_ObjectType.GetCurSel(); 
            strcpy(stObjectInfo.aObjectType, stObjectConfig.aObjectType[stObjectInfo.iObjectIndex]);

			stObjectInfo.iObjectPartIndex = m_pObjectAnnoDlg->m_ComboCtrl_ObjectPartType.GetCurSel();
			if (stObjectInfo.iObjectPartIndex != -1)
			{
				stObjectInfo.bObjectPart = TRUE;
				strcpy(stObjectInfo.aObjectPartType, stObjectConfig.aObjectPartType[stObjectInfo.iObjectPartIndex]);
			}
           
            
            memcpy(stObjectInfo.aObjectAreaLabelMap, m_iObjectLabelMap, OBJECT_BLOCK_X_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));

			if (-1 != m_iObjectSel)
			{
				memcpy(stObjectInfo.stObjectRectPt, m_vObjInfo[m_iObjectSel].stObjectRectPt, 4 * sizeof(CvPoint));

				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_iObjectSel, 1, stObjectInfo.aObjectType);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_iObjectSel, 2, stObjectInfo.aObjectPartType);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_iObjectSel, 3, stObjectConfig.aObjectColor[stObjectInfo.ObjectColor]);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_iObjectSel, 4, stObjectInfo.aObjectContext);

				m_vObjInfo[m_iObjectSel] = stObjectInfo;
			}
			else
			{
				memcpy(stObjectInfo.stObjectRectPt, m_ObjModifyPt, 4 * sizeof(CvPoint));

				char cBuf[20];
				sprintf(cBuf, "%d", m_nObjAnnoNum + 1);

				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.InsertItem(m_nObjAnnoNum, cBuf);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_nObjAnnoNum, 1, stObjectInfo.aObjectType);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_nObjAnnoNum, 2, stObjectInfo.aObjectPartType);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_nObjAnnoNum, 3, stObjectConfig.aObjectColor[stObjectInfo.ObjectColor]);
				m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(m_nObjAnnoNum, 4, stObjectInfo.aObjectContext);

				m_nObjAnnoNum++;
				m_vObjInfo.push_back(stObjectInfo);
			}

            ClearVariableState();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //画已经标注的目标
	if(!m_vObjInfo.empty())
	{
		for (int i = 0; i < m_nObjAnnoNum; i++)
		{
			//EnterCriticalSection(&m_PlayCriticalSection);

            ObjectInfo stTempObjectInfo = m_vObjInfo[i];

			//LeaveCriticalSection(&m_PlayCriticalSection);

			CvScalar color = RED;
			if (i == m_iObjectSel)
			{
				color = GREEN;
			}

            CvPoint stLTPt = stTempObjectInfo.stObjectRectPt[0];
            CvPoint stRBPt = stTempObjectInfo.stObjectRectPt[2];

			int iStepX, iStepY;
			if (stTempObjectInfo.iDrawType == 0)
			{
				iStepX = (stRBPt.x - stLTPt.x) / OBJECT_BLOCK_X_NUM;
				iStepY = (stRBPt.y - stLTPt.y) / OBJECT_BLOCK_Y_NUM;

				int iX = (stRBPt.x - stLTPt.x) % iStepX;
				int iY = (stRBPt.y - stLTPt.y) % iStepY;
				
				if (iX > 0 && iX < (iStepX / 2))
				{
					stLTPt.x += iX / 2;
					stRBPt.x -= iX / 2;
				}
				else if (iX >= (iStepX / 2))
				{
					stLTPt.x -= (iStepX - iX) / 2;
					stRBPt.x += (iStepX - iX) / 2;
					iStepX = (stRBPt.x - stLTPt.x) / OBJECT_BLOCK_X_NUM;
				}

				if (iY > 0 && iY < (iStepY / 2))
				{
					stLTPt.y += iY / 2;
					stRBPt.y -= iY / 2;
				}
				else if (iY >= (iStepY / 2))
				{
					stLTPt.y -= (iStepY - iY) / 2;
					stRBPt.y += (iStepY - iY) / 2;
					iStepY = (stRBPt.y - stLTPt.y) / OBJECT_BLOCK_Y_NUM;
				}

				stRBPt.x = stLTPt.x + iStepX * OBJECT_BLOCK_X_NUM;
				stRBPt.y = stLTPt.y + iStepY * OBJECT_BLOCK_Y_NUM;

				stTempObjectInfo.stObjectRectPt[0] = stLTPt;
				stTempObjectInfo.stObjectRectPt[1] = cvPoint(stRBPt.x, stLTPt.y);
				stTempObjectInfo.stObjectRectPt[2] = stRBPt;
				stTempObjectInfo.stObjectRectPt[3] = cvPoint(stLTPt.x, stRBPt.y);
			}
			
			for (int k = 0; k < 3; k++)
			{
				cvLine(img, stTempObjectInfo.stObjectRectPt[k], stTempObjectInfo.stObjectRectPt[k + 1], color, 1, 8, 0);
				if (k == 0)
				{
					cvLine(img, stTempObjectInfo.stObjectRectPt[0], stTempObjectInfo.stObjectRectPt[3], color, 1, 8, 0);
				}
			}
            
            if (/*m_bCheckMode && */stTempObjectInfo.iDrawType == 0 && i == m_iObjectSel)
            {
				memcpy(m_ObjModifyPt, stTempObjectInfo.stObjectRectPt, 4 * sizeof(CvPoint));

				m_ObjLTPt = stLTPt;
				m_ObjRBPt = stRBPt;
				m_iStepX = iStepX;
				m_iStepY = iStepY;
            }
		}
	}

    m_pObjectAnnoDlg->DrawPic2HDC(img, IDC_Static_ImageShow);

	cvReleaseImage(&img);
}

void DrawObjPic::ReDrawPic()
{
	m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.DeleteAllItems();

    m_ObjPos    = cvPoint(0, 0);
    m_ObjCurPos = cvPoint(0, 0);
    m_ObjLTPt   = cvPoint(0, 0);
    m_ObjRBPt   = cvPoint(0, 0);

    m_bObjRectDrawed = false;
    m_bObjectInfoSaved = false;

    m_iObjectSel = -1;
    //m_iObjectAngleX = 0;
    //m_iObjectAngleY = 0;

    ClearObjectInfo();

    memset(m_ObjModifyPt, 0, 4 * sizeof(CvPoint));
    memset(m_iObjectLabelMap, -1, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));

	if(m_nObjAnnoNum > 0)
	{
        m_vObjInfo.clear();
        m_nObjAnnoNum = 0;
	}
	
	return;
}

void DrawObjPic::WriteObjectAnnoInfo()
{
	if(m_nObjAnnoNum == 0)
	{
		return;
	}
	
    CString strFileName = m_vFileName[m_nFileIndex];
    strFileName.ReleaseBuffer();
    strFileName.Delete(strFileName.GetLength() - 4, 4);

    CString strTxtPath;

    if (FileType == 1)
    {
        strTxtPath = CString(m_aFilePath) + strFileName + CString(".xml");
    }
    else if(FileType == 2)
	{
        CString strTxtDir = CString(m_aFilePath) + strFileName;

        if (GetFileAttributes(strTxtDir) != FILE_ATTRIBUTE_DIRECTORY)
        {
            CreateDirectory(strTxtDir, NULL);
        }

        EnterCriticalSection(&m_PlayCriticalSection);

        char aFileName[20];
        sprintf(aFileName, "%04d.jpg", m_nVideoFrameIdx);
        CString strImgPath = strTxtDir + CString("\\") + CString(aFileName);

        sprintf(aFileName, "%04d.txt", m_nVideoFrameIdx);
        strTxtPath = strTxtDir + CString("\\") + CString(aFileName);

        cvSaveImage(strImgPath.GetBuffer(), m_pSrcImage);

        LeaveCriticalSection(&m_PlayCriticalSection);
	}

	CvFileStorage *fs = cvOpenFileStorage(strTxtPath.GetBuffer(), 0, CV_STORAGE_WRITE, "GB2312");

	if (!fs)
	{
		AfxMessageBox("obj info xml file open write failed~!");
		return;
	}

	cvStartWriteStruct(fs, "ObjectInfo", CV_NODE_MAP);
	cvWriteComment(fs, "标注目标信息", 1);

	cvWriteInt(fs, "ObjectNum", m_nObjAnnoNum);

	for (int i = 0; i < m_vObjInfo.size(); i++)
	{
		ObjectInfo stTempObjInfo = m_vObjInfo[i];

		for (int k = 0; k < 4; k++)
		{
			if (m_bPlayRectWnd)
			{
				stTempObjInfo.stObjectRectPt[k].x = cvRound(m_PlayRect.x + stTempObjInfo.stObjectRectPt[k].x * m_fRectWidthScale);
				stTempObjInfo.stObjectRectPt[k].y = cvRound(m_PlayRect.y + stTempObjInfo.stObjectRectPt[k].y * m_fRectHeightScale);
			}
			else
			{
				stTempObjInfo.stObjectRectPt[k].x = cvRound((stTempObjInfo.stObjectRectPt[k].x - m_stShowROI.x) * m_fWidthScale);
				stTempObjInfo.stObjectRectPt[k].y = cvRound((stTempObjInfo.stObjectRectPt[k].y - m_stShowROI.y) * m_fHeightScale);
			}
		}

		char szObject[_MAX_PATH];
		sprintf_s(szObject, "Object_%d", i + 1);

		cvStartWriteStruct(fs, szObject, CV_NODE_MAP);

		memset(szObject, 0, _MAX_PATH*sizeof(char));
		for (int k = 0; k < g_DrawObjPic.stObjectConfig.iObjectNum; k++)
		{
			char szText[40];
			sprintf_s(szText, "%d:%s ", k, g_DrawObjPic.stObjectConfig.aObjectType[k]);
			strcat_s(szObject, szText);
		}

		cvWriteComment(fs, szObject, 0);
		cvWriteInt(fs, "ObjectType", stTempObjInfo.iObjectIndex);

		cvWriteInt(fs, "IsHavePart", stTempObjInfo.bObjectPart);
		if (stTempObjInfo.bObjectPart)
		{
			memset(szObject, 0, _MAX_PATH*sizeof(char));
			for (int k = 0; k < g_DrawObjPic.stObjectConfig.iObjectPartNum; k++)
			{
				char szText[40];
				sprintf_s(szText, "%d:%s ", k, g_DrawObjPic.stObjectConfig.aObjectPartType[k]);
				strcat_s(szObject, szText);
			}

			cvWriteComment(fs, szObject, 0);
			cvWriteInt(fs, "ObjectPartType", stTempObjInfo.iObjectPartIndex);
		}

		sprintf_s(szObject, "-1:unkown");
		for (int k = 0; k < g_DrawObjPic.stObjectConfig.iObjectColorNum; k++)
		{
			char szText[40];
			sprintf_s(szText, " %d:%s", k, g_DrawObjPic.stObjectConfig.aObjectColor[k]);
			strcat_s(szObject, szText);
		}

		cvWriteComment(fs, szObject, 0);
		cvWriteInt(fs, "ObjectColor", stTempObjInfo.ObjectColor);
		
		cvWriteComment(fs, "0:栅格 1：平行四边形", 0);
		cvWriteInt(fs, "AnnStyle", stTempObjInfo.iDrawType);

		cvWriteInt(fs, "IsHaveContext", stTempObjInfo.bHaveContext);

		if (stTempObjInfo.bHaveContext)
		{
			cvWriteString(fs, "ObjectContext", stTempObjInfo.aObjectContext);
		}

		cvWriteInt(fs, "ObjectAngleY", stTempObjInfo.iObjectAngleY);

		cvWriteInt(fs, "ObjectAngleX", stTempObjInfo.iObjectAngleX);
		
		cvWriteComment(fs, "目标标注坐标", 0);
		for (int k = 0; k < 4; k++)
		{
			char szPt[20];
			sprintf_s(szPt, "X_%d", k + 1);
			cvWriteInt(fs, szPt, stTempObjInfo.stObjectRectPt[k].x);
			sprintf_s(szPt, "Y_%d", k + 1);
			cvWriteInt(fs, szPt, stTempObjInfo.stObjectRectPt[k].y);
		}

		if (!stTempObjInfo.iDrawType)
		{
			cvWriteComment(fs, "目标标注前背景信息", 0);
			cvWriteInt(fs, "Block_X_Num", OBJECT_BLOCK_X_NUM);

			cvWriteInt(fs, "Block_Y_Num", OBJECT_BLOCK_Y_NUM);
			cvWriteComment(fs, "0:前景 1：背景", 0);
			
			cvStartWriteStruct(fs, "FG_BG", CV_NODE_SEQ );
			cvWriteRawData(fs, stTempObjInfo.aObjectAreaLabelMap, OBJECT_BLOCK_X_NUM*OBJECT_BLOCK_Y_NUM, "i");
			cvEndWriteStruct(fs);
		}

		cvEndWriteStruct(fs);
	}

	cvEndWriteStruct(fs);
	
	cvReleaseFileStorage(&fs);
}

void DrawObjPic::ReadObjectAnnoInfo()
{
    CString strFileName = m_vFileName[m_nFileIndex];
    strFileName.ReleaseBuffer();
    strFileName.Delete(strFileName.GetLength() - 4, 4);

    std::string strTxtPath = std::string(m_aFilePath) + strFileName.GetBuffer() + std::string(".xml");

	m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.DeleteAllItems();

	//read xml file
	CvFileStorage *fs = cvOpenFileStorage(strTxtPath.c_str(), 0, CV_STORAGE_READ);
	if (!fs)
	{
		AfxMessageBox("obj info xml file read failed~!");
		return;
	}

	ObjectInfo stTempObjInfo;
	memset(&stTempObjInfo, 0, sizeof(ObjectInfo));
	memset(stTempObjInfo.aObjectAreaLabelMap, -1, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));

	CvSeqReader reader;
	CvFileNode *root = cvGetRootFileNode(fs, 0);
	CvFileNode *node = cvGetFileNodeByName(fs, root, "ObjectInfo");

	cvStartReadSeq(node->data.seq, &reader);

	m_nObjAnnoNum = cvReadIntByName(fs, node, "ObjectNum");

	for (int k = 0; k < m_nObjAnnoNum; k++)
	{
		char szObject[40];
		sprintf_s(szObject, "Object_%d", k + 1);
		CvFileNode *tempNode = cvGetFileNodeByName(fs, node, szObject);

		cvStartReadSeq(tempNode->data.seq, &reader);

		stTempObjInfo.iObjectIndex = cvReadIntByName(fs, tempNode, "ObjectType");
		strcpy(stTempObjInfo.aObjectType, g_DrawObjPic.stObjectConfig.aObjectType[stTempObjInfo.iObjectIndex]);

		stTempObjInfo.bObjectPart = cvReadIntByName(fs, tempNode, "IsHavePart");

		stTempObjInfo.iObjectPartIndex = -1;
		if (stTempObjInfo.bObjectPart)
		{
			stTempObjInfo.iObjectPartIndex = cvReadIntByName(fs, tempNode, "ObjectPartType");
			strcpy(stTempObjInfo.aObjectPartType, g_DrawObjPic.stObjectConfig.aObjectPartType[stTempObjInfo.iObjectPartIndex]);
		}

		stTempObjInfo.ObjectColor = (enObjectColor)cvReadIntByName(fs, tempNode, "ObjectColor");
		stTempObjInfo.iDrawType = cvReadIntByName(fs, tempNode, "AnnStyle");
		stTempObjInfo.bHaveContext = cvReadIntByName(fs, tempNode, "IsHaveContext");

		if (stTempObjInfo.bHaveContext)
		{
			strcpy(stTempObjInfo.aObjectContext, cvReadStringByName(fs, tempNode, "ObjectContext"));
		}

		stTempObjInfo.iObjectAngleY = cvReadIntByName(fs, tempNode, "ObjectAngleY");
		stTempObjInfo.iObjectAngleX = cvReadIntByName(fs, tempNode, "ObjectAngleX");

		for (int p = 0; p < 4; p++)
		{
			sprintf_s(szObject, "X_%d", p + 1);
			stTempObjInfo.stObjectRectPt[p].x = cvReadIntByName(fs, tempNode, szObject);

			sprintf_s(szObject, "Y_%d", p + 1);
			stTempObjInfo.stObjectRectPt[p].y = cvReadIntByName(fs, tempNode, szObject);
		}

		for (int k = 0; k < 4; k++)
		{
			stTempObjInfo.stObjectRectPt[k].x = cvRound(m_stShowROI.x + stTempObjInfo.stObjectRectPt[k].x / m_fWidthScale);
			stTempObjInfo.stObjectRectPt[k].y = cvRound(m_stShowROI.y + stTempObjInfo.stObjectRectPt[k].y / m_fHeightScale);
		}

		if (!stTempObjInfo.iDrawType)
		{
			CvFileNode *subnode = cvGetFileNodeByName(fs, tempNode, "FG_BG");
			cvReadRawData(fs, subnode, stTempObjInfo.aObjectAreaLabelMap, "i");
		}

		char cBuf[20];
		sprintf(cBuf, "%d", k + 1);

		m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.InsertItem(k, cBuf);
		m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(k, 1, stTempObjInfo.aObjectType);
		m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(k, 2, stTempObjInfo.aObjectPartType);
		m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(k, 3, stObjectConfig.aObjectColor[stTempObjInfo.ObjectColor]);
		m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(k, 4, stTempObjInfo.aObjectContext);

		m_pObjectAnnoDlg->m_ComboCtrl_DrawStyle.SetCurSel(m_iDrawStyle);

		m_vObjInfo.push_back(stTempObjInfo);
	}

	cvReleaseFileStorage(&fs);
}

void DrawObjPic::LoadNextFile()
{
    if (m_nObjAnnoNum > 0 && !m_bObjectInfoSaved)
    {
        int iRet = AfxMessageBox(_T("是否保存上一个标注信息?"), MB_YESNO);

        if (iRet == IDYES)
        {
            WriteObjectAnnoInfo();
        }
    }

	if(m_nFileCnt == 1)
	{
		m_nFileIndex = 0;
	}
	else
	{
		if(m_nFileIndex == m_nFileCnt - 1)
		{
            AfxMessageBox(_T("所有文件已经标注完成~!"));
            return;
		}
		else
		{
		    m_nFileIndex++;
		}
	}

    ReDrawPic();

    std::string strFilePath = std::string(m_aFilePath) + m_vFileName[m_nFileIndex].GetBuffer(0); 

    if (FileType == 1)
    {
        m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetPos(m_nFileIndex + 1);

        CString strBuf;
        strBuf.Format("当前图片进度: %d / %d", m_nFileIndex + 1, m_vFileName.size());
        m_pObjectAnnoDlg->m_Static_FileProcess.SetWindowText(strBuf);

		CString strCaption = CString("目标标注 - ") + strFilePath.c_str();
		m_pObjectAnnoDlg->SetWindowText(strCaption);

        ReadImageFile((char*)strFilePath.c_str());
    }
    else if (FileType == 2)
    {
        EnterCriticalSection(&m_PlayCriticalSection);

        strcpy(m_strCurFilePath, strFilePath.c_str());

        m_bRunNextFile = true;

        Sleep(10);

        LeaveCriticalSection(&m_PlayCriticalSection);
    }
}

void DrawObjPic::LoadPrevFile()
{
    if (m_nObjAnnoNum > 0 && !m_bObjectInfoSaved)
    {
        int iRet = AfxMessageBox(_T("是否保存上一个标注信息?"), MB_YESNO);

        if (iRet == IDYES)
        {
            WriteObjectAnnoInfo();
        }
    }

	if(m_nFileCnt == 1)
	{
		m_nFileIndex = 0;
	}
	else
	{
		if(m_nFileIndex == 0)
		{
            AfxMessageBox(_T("已经到达文件起始位置~!"));
            return;
		}

		m_nFileIndex--;
	}

    ReDrawPic();

    std::string strFilePath = std::string(m_aFilePath) + m_vFileName[m_nFileIndex].GetBuffer(0); 
	
    if (FileType == 1)
    {
        m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetPos(m_nFileIndex + 1);

        CString strBuf;
        strBuf.Format("当前图片进度: %d / %d", m_nFileIndex + 1, m_vFileName.size());
        m_pObjectAnnoDlg->m_Static_FileProcess.SetWindowText(strBuf);

		CString strCaption = CString("目标标注 - ") + strFilePath.c_str();
		m_pObjectAnnoDlg->SetWindowText(strCaption);

        ReadImageFile((char*)strFilePath.c_str());
    }
    else if (FileType == 2)
    {
        EnterCriticalSection(&m_PlayCriticalSection);

        strcpy(m_strCurFilePath, strFilePath.c_str());

        m_bRunNextFile = true;

        Sleep(10);

        LeaveCriticalSection(&m_PlayCriticalSection);
    }
}

void DrawObjPic::UndoDrawObj()
{
	m_ObjPos    = cvPoint(0, 0);
	m_ObjCurPos = cvPoint(0, 0);
    m_ObjLTPt   = cvPoint(0, 0);
    m_ObjRBPt   = cvPoint(0, 0);

    //m_iObjectSel = -1;
    //m_iObjectAngleX = 0;
    //m_iObjectAngleY = 0;

    m_bObjectInfoSaved = false;

    ClearObjectInfo();

    memset(m_ObjModifyPt, 0, 4 * sizeof(CvPoint));
    memset(m_iObjectLabelMap, -1, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));

    if (m_bObjRectDrawed)
    {
        m_bObjRectDrawed = false;
        return;
    }

	if(m_nObjAnnoNum == 0)
	{
		return;
	}

    m_pObjectAnnoDlg->m_ListCtrl_ObjectInfo.DeleteItem(m_nObjAnnoNum - 1);

	m_vObjInfo.pop_back();
	m_nObjAnnoNum--;
}

void DrawObjPic::RealPlay()
{
    if (m_nObjAnnoNum > 0 && !m_bObjectInfoSaved)
    {
        int iRet = AfxMessageBox(_T("是否保存上一个标注信息?"), MB_YESNO);

        if (iRet == IDYES)
        {
            WriteObjectAnnoInfo();
        }
    }

    ReDrawPic();

	m_bPause = false;
	m_ObjPos = cvPoint(0, 0);
	m_ObjCurPos = cvPoint(0, 0);
}

void DrawObjPic::StopPlay()
{
	m_bPause = true;
	m_ObjPos = cvPoint(0, 0);
	m_ObjCurPos = cvPoint(0, 0);
}

void DrawObjPic::RealPlayRect()
{
	m_bPlayRect = true;
}

void DrawObjPic::ReRealPlay()
{
	m_bPlayRect      = false;
	m_bPlayRectWnd   = false;
    m_bModifyObjRect = false;
    m_bModifyObjLabel = false;
    m_bObjRectDrawed = false;

	m_ObjPos    = cvPoint(0, 0);
	m_ObjCurPos = cvPoint(0, 0);

	PostMessage(m_hWnd, WM_MSG_CAR, 1, 0);

	m_nCtrlPlaySpeed = 200;
}

void DrawObjPic::PlayAccelerate() //加速
{
	if(m_nCtrlPlaySpeed < 20)
	{
		return;
	}

	m_nCtrlPlaySpeed = m_nCtrlPlaySpeed - 20;
	if(m_nCtrlPlaySpeed < 20)
	{
		m_nCtrlPlaySpeed = 20;
	}

	return;
}

void DrawObjPic::PlayDecelerate() //减速
{
	if(m_nCtrlPlaySpeed < 20)
	{
		return;
	}

	m_nCtrlPlaySpeed = m_nCtrlPlaySpeed + 20;

	return;
}

void DrawObjPic::ExitThread()
{
    m_bRunNextFile = true;
	m_bExitThread  = true;

	WaitForSingleObject(m_hObjThread, INFINITE);

	DWORD iExit, m_hiExit;

	GetExitCodeThread(m_hObjThread, &iExit);

	::TerminateProcess(m_hObjThread, iExit);

	CloseHandle(m_hObjThread);

	m_hObjThread = NULL;

	WaitForSingleObject(m_hCaptureThread,INFINITE);

	GetExitCodeThread(m_hCaptureThread,&m_hiExit);

	::TerminateProcess(m_hCaptureThread, m_hiExit);

	CloseHandle(m_hCaptureThread);

	m_hCaptureThread = NULL;
}

void DrawObjPic::ShowRectZoomImage()  //将一片小区域放大
{
	int iZoomImageWidth = 100;
	int iZoomImageHeight = 80;

    CPoint Pt = CPoint(m_CurCursorPos.x, m_CurCursorPos.y);

    if (!PtInRect(&m_ImageRect, Pt) || m_ImageShow == NULL)
    {
        return;
    }
    
    int iRectWidth = (m_ImageRect.Width() / 4) * 4;
    int iRectheight = m_ImageRect.Height();

	IplImage *pZoomRectImage = cvCreateImage(cvSize(iRectWidth, iRectheight), IPL_DEPTH_8U, 3);

    EnterCriticalSection(&m_PlayCriticalSection);

	memcpy(pZoomRectImage->imageData, (char *)m_ImageShow, iRectWidth * iRectheight * 3 * sizeof(char));

    LeaveCriticalSection(&m_PlayCriticalSection);

	int x1 = max(0, Pt.x - iZoomImageWidth / 2);
	int y1 = max(0, Pt.y - iZoomImageHeight / 2);
	int x2 = min(iRectWidth - 1, Pt.x + iZoomImageWidth / 2);
	int y2 = min(iRectheight - 1, Pt.y + iZoomImageHeight / 2);

	CvRect stZoomRect = cvRect(x1, y1, x2 - x1, y2 - y1);
	CvRect stZoomROI = cvRect(0, 0, Image_Zoom_Width, Image_Zoom_Height);

	x1 = Pt.x - iZoomImageWidth / 2;
	y1 = Pt.y - iZoomImageHeight / 2;
	x2 = Pt.x + iZoomImageWidth / 2 - m_ImageRect.Width();
	y2 = Pt.y + iZoomImageHeight / 2 - iRectheight;
	
	//left side
	if (x1 < 0 && y1 >= 0)
	{
		int x = min(abs(x1) * 3, Image_Zoom_Width);
		stZoomROI = cvRect(x, 0, Image_Zoom_Width - x, Image_Zoom_Height);
	}
	//top side
	if (y1 < 0 && x1 >= 0)
	{
		int y = min(abs(y1) * 3, Image_Zoom_Height);
		stZoomROI = cvRect(0, y, Image_Zoom_Width, Image_Zoom_Height - y);
	}
	//left top
	if (x1 < 0 && y1 < 0)
	{
		int x = min(abs(x1) * 3, Image_Zoom_Width);
		int y = min(abs(y1) * 3, Image_Zoom_Height);
		stZoomROI = cvRect(x, y, Image_Zoom_Width - x, Image_Zoom_Height - y);
	}
	//right side
	if (x2 > 0 && y2 <= 0)
	{
		int x = min(x2 * 3, Image_Zoom_Width);
		stZoomROI = cvRect(0, 0, Image_Zoom_Width - x, Image_Zoom_Height);
	}
	//bottom side
	if (y2 > 0 && x2 <= 0)
	{
		int y = min(y2 * 3, Image_Zoom_Height);
		stZoomROI = cvRect(0, 0, Image_Zoom_Width, Image_Zoom_Height - y);
	}
	//right bottom side
	if (x2 > 0 && y2 > 0)
	{
		int x = min(x2 * 3, Image_Zoom_Width);
		int y = min(y2 * 3, Image_Zoom_Height);
		stZoomROI = cvRect(0, 0, Image_Zoom_Width - x, Image_Zoom_Height - y);
	}

	if (y1 < 0 && x2 > 0)
	{
		int y = min(abs(y1) * 3, Image_Zoom_Height);
		int x = min(x2 * 3, Image_Zoom_Width);
		stZoomROI = cvRect(0, y, Image_Zoom_Width - x, Image_Zoom_Height - y);
	}

	if (m_bObjRectDrawed && m_bPause)
	{
		DrawObjectRectOrGrid(pZoomRectImage);
	}

	cvZero(m_pImageZoomShow);

	cvSetImageROI(pZoomRectImage, stZoomRect);
	cvSetImageROI(m_pImageZoomShow, stZoomROI);

    cvResize(pZoomRectImage, m_pImageZoomShow, CV_INTER_LINEAR);

    cvResetImageROI(pZoomRectImage);
	cvResetImageROI(m_pImageZoomShow);

	cvRectangle(m_pImageZoomShow, cvPoint(150, 120), cvPoint(150, 120), WHITE, 7);

	cvLine(m_pImageZoomShow, cvPoint(150, 0), cvPoint(150, 240), PINK, 1);
	cvLine(m_pImageZoomShow, cvPoint(0, 120), cvPoint(300, 120), PINK, 1);
    
    std::string strWinName = "Zoom-Image";
	cvNamedWindow(strWinName.c_str(), 1);
	cvShowImage(strWinName.c_str(), m_pImageZoomShow);

	m_hZoomWnd = FindWindow(NULL, strWinName.c_str());

	BringWindowToTop(m_hZoomWnd);

    cvReleaseImage(&pZoomRectImage);
}

void DrawObjPic::ReadImageFile(char *pImgPath)
{
    if(m_hZoomWnd != NULL)
    {
        cvDestroyWindow("Zoom-Image");
        m_hZoomWnd = NULL;
    }

    IplImage *img = cvLoadImage(pImgPath, CV_LOAD_IMAGE_UNCHANGED);

    if (NULL == img)
    {
        AfxMessageBox("图像文件打开失败");
        return;
    }

    int iRectWidth  = (m_ImageRect.Width() / 4) * 4;
    int iRectHeight = m_ImageRect.Height();

    //m_fWidthScale = ((float)img->width) / ((float)iRectWidth);
    //m_fHeightScale = ((float)img->height) / ((float)iRectHeight);

    m_PlayRect = cvRect(0, 0, m_OriImageWidth, m_OriImageHeight);

    m_OriImageWidth = img->width;
    m_OriImageHeight = img->height;

	int iResizeW, iResizeH;   
	float ftp = (float)iRectWidth / (float)iRectHeight;
	float fRatio = (float)m_OriImageWidth / (float)m_OriImageHeight;

	if (fRatio > ftp)
	{
		iResizeW = iRectWidth;
		iResizeH = iResizeW / fRatio;
	}
	else
	{
		iResizeH = iRectHeight;
		iResizeW = iResizeH * fRatio;
	}

	CvRect stRect = cvRect((iRectWidth - iResizeW) / 2, (iRectHeight - iResizeH) / 2, iResizeW, iResizeH);
	m_stShowROI = stRect;

	m_fWidthScale = ((float)img->width) / ((float)stRect.width);
	m_fHeightScale = ((float)img->height) / ((float)stRect.height);

	if (m_bCheckMode)
	{
		EnterCriticalSection(&m_PlayCriticalSection);

		ReadObjectAnnoInfo();

		LeaveCriticalSection(&m_PlayCriticalSection);

		//Sleep(100);
	}

	EnterCriticalSection(&m_PlayCriticalSection);

    if(m_ImageShow!=NULL)
    {
        delete m_ImageShow;
        m_ImageShow = NULL;
    }

    m_ImageShow = new char[iRectWidth * iRectHeight * 3];
    memset(m_ImageShow, 0, iRectWidth * iRectHeight * 3 * sizeof(char));

    IplImage *pImg = cvCreateImageHeader(cvSize(iRectWidth, iRectHeight), IPL_DEPTH_8U, 3);

    pImg->imageData = m_ImageShow;

	cvSetImageROI(pImg, stRect);

    cvResize(img, pImg, CV_INTER_LINEAR);

	cvResetImageROI(pImg);

	cvRectangle(pImg, cvPoint(stRect.x, stRect.y), cvPoint(stRect.x + stRect.width, stRect.y + stRect.height), 
		GREEN, 2, 8, 0);

    cvReleaseImageHeader(&pImg);
    cvReleaseImage(&img);

    LeaveCriticalSection(&m_PlayCriticalSection);
}

void DrawObjPic::ReadVideoFile()
{
    while(!m_bExitThread)
    {
        CvCapture *pVideoCapture = cvCaptureFromAVI(m_strCurFilePath);

        if(NULL == pVideoCapture)
        {
            AfxMessageBox("视频文件打开失败");
            return;
        }

        m_bRunNextFile = false;

        int iRectWidth  = (m_ImageRect.Width() / 4) * 4;
        int iRectHeight = m_ImageRect.Height();

        m_nVideoFrameNum = cvGetCaptureProperty(pVideoCapture, CV_CAP_PROP_FRAME_COUNT);//视频总帧数
        m_nVideoFrameIdx = cvGetCaptureProperty(pVideoCapture, CV_CAP_PROP_POS_FRAMES);//当前视频帧
        m_OriImageWidth  = cvGetCaptureProperty(pVideoCapture, CV_CAP_PROP_FRAME_WIDTH);
        m_OriImageHeight = cvGetCaptureProperty(pVideoCapture, CV_CAP_PROP_FRAME_HEIGHT);

        m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetRange(1, m_nVideoFrameNum);
        m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetPos(1);

        CString strBuf;
        strBuf.Format("当前视频帧进度: %d / %d", m_nVideoFrameIdx + 1, m_nVideoFrameNum);
        m_pObjectAnnoDlg->m_Static_FileProcess.SetWindowText(strBuf);

        m_fWidthScale  = ((float)m_OriImageWidth) / ((float)iRectWidth);
        m_fHeightScale = ((float)m_OriImageHeight) / ((float)iRectHeight);

        m_bPause = false;

        if(m_ImageShow != NULL)
        {
            delete m_ImageShow;
            m_ImageShow = NULL;
        }

        m_ImageShow = new char[iRectWidth * iRectHeight * 3];
        memset(m_ImageShow, 0, iRectWidth * iRectHeight * 3 * sizeof(char));

        while(!m_bRunNextFile)
        {
            EnterCriticalSection(&m_PlayCriticalSection);

            m_PlayRect = cvRect(0, 0, m_OriImageWidth, m_OriImageHeight);

            if(m_bPlayRect && m_bPlayRectWnd)
            {
                CvPoint stPlayPtTL = cvPoint(m_PlayPointTL.x * m_fWidthScale, m_PlayPointTL.y * m_fHeightScale);
                CvPoint stPlayPtBR = cvPoint(m_PlayPointBR.x * m_fWidthScale, m_PlayPointBR.y * m_fHeightScale);

                int iPlayRectW = ((stPlayPtBR.x - stPlayPtTL.x) / 4) * 4;
                int iPlayRectH = (int)(stPlayPtBR.y - stPlayPtTL.y);

                m_fRectWidthScale = ((float)iPlayRectW) / ((float)iRectWidth);
                m_fRectHeightScale = ((float)iPlayRectH) / ((float)iRectHeight);

                m_PlayRect = cvRect(stPlayPtTL.x, stPlayPtTL.y, iPlayRectW, iPlayRectH);
            }

            LeaveCriticalSection(&m_PlayCriticalSection);

            if(!m_bPause)
            {
                if(m_hZoomWnd != NULL)
                {
                    cvDestroyWindow("Zoom-Image");
                    m_hZoomWnd = NULL;
                }

                EnterCriticalSection(&m_PlayCriticalSection);
                
                if (m_bSliderChanged)
                {
                    cvSetCaptureProperty(pVideoCapture, CV_CAP_PROP_POS_FRAMES, m_nVideoFrameIdx);

                    m_bSliderChanged = false;
                }

                m_nVideoFrameIdx = cvGetCaptureProperty(pVideoCapture, CV_CAP_PROP_POS_FRAMES);//当前视频帧

                m_pObjectAnnoDlg->m_SliderCtrl_FileProgress.SetPos(m_nVideoFrameIdx + 1);
                strBuf.Format("当前视频帧进度: %d / %d", m_nVideoFrameIdx + 1, m_nVideoFrameNum);
                m_pObjectAnnoDlg->m_Static_FileProcess.SetWindowText(strBuf);

                if(m_bPlayForward)
                {
                    if(m_nVideoFrameIdx <= m_nVideoFrameNum - 10)
                        cvSetCaptureProperty(pVideoCapture, CV_CAP_PROP_POS_FRAMES, m_nVideoFrameIdx + 10);
                    m_bPlayForward = false;
                }

                if(m_bPlayBackward)
                {
                    if(m_nVideoFrameIdx >= 10)
                        cvSetCaptureProperty(pVideoCapture, CV_CAP_PROP_POS_FRAMES, m_nVideoFrameIdx - 10);
                    m_bPlayBackward = false;
                }
                
                if(m_nVideoFrameIdx == m_nVideoFrameNum)
                {
                    LeaveCriticalSection(&m_PlayCriticalSection);
                    break;
                }

                m_pSrcImage = cvQueryFrame(pVideoCapture);

                if(m_pSrcImage->origin == IPL_ORIGIN_BL)
                {
                    cvFlip(m_pSrcImage);
                }

                IplImage *pRectImage = cvCreateImageHeader(cvSize(iRectWidth, iRectHeight), IPL_DEPTH_8U, 3);

                pRectImage->imageData = m_ImageShow;

                cvSetImageROI(m_pSrcImage, m_PlayRect);

                cvResize(m_pSrcImage, pRectImage, CV_INTER_LINEAR);

                cvResetImageROI(m_pSrcImage);

                cvReleaseImageHeader(&pRectImage);

                PostMessage(m_hWnd, WM_MSG_CAR, 1, 0);

                LeaveCriticalSection(&m_PlayCriticalSection);
            }

            Sleep(m_nCtrlPlaySpeed);
        }

        cvReleaseCapture(&pVideoCapture);
        pVideoCapture = NULL;
    }
}

void DrawObjPic::ShowObjectModel(int iAngleX, int iAngleY)
{
    int iObjectSel = m_pObjectAnnoDlg->m_ComboCtrl_ObjectType.GetCurSel();
    
    if (-1 == iObjectSel)
    {
        AfxMessageBox(_T("请先选择标注目标类型"));
        return;
    }

	int iObjectPartiSel = m_pObjectAnnoDlg->m_ComboCtrl_ObjectPartType.GetCurSel();

	if (iObjectPartiSel == 2)
	{
		return;
	}
    
    //strcpy(stObjectInfo.aObjectType, stObjectConfig.aObjectType[stObjectInfo.iObjectIndex]);

    char ModelName[20];
    sprintf(ModelName, "%d_%d.png", iAngleY, iAngleX);

    std::string strModelPath = std::string(m_aExePath) + std::string("VehicleModel\\")
        + std::string(stObjectConfig.aObjectType[iObjectSel]) + std::string("\\") + ModelName;

    IplImage *pModelImage = cvLoadImage(strModelPath.c_str(), CV_LOAD_IMAGE_COLOR);

    if (pModelImage == NULL)
    {
        return;
    }

    IplImage *pModelResizeImage = cvCreateImage(cvSize(400, 300), IPL_DEPTH_8U, 3);

    cvResize(pModelImage, pModelResizeImage, CV_INTER_LINEAR);

    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1.5, 8);

    char angleText[30];
    sprintf(angleText, "Tilt : %d  Pan : %d", iAngleY, iAngleX);

    cvPutText(pModelResizeImage, angleText, cvPoint(20, 20), &font, cvScalar(255, 255));

    cvNamedWindow("ModelImage", 1);
    cvShowImage("ModelImage", pModelResizeImage);

    HWND hWnd = FindWindow(NULL, "ModelImage");

    BringWindowToTop(hWnd);

    cvReleaseImage(&pModelImage);
    cvReleaseImage(&pModelResizeImage);

    stObjectInfo.iObjectAngleX = iAngleX;
    stObjectInfo.iObjectAngleY = iAngleY;
}

void DrawObjPic::ReCreateThread(void)
{
    if(!m_bExitThread)
    {
        m_bRunNextFile    = true;
        m_bExitThread     = true;

        ::WaitForSingleObject(m_hObjThread, INFINITE);
        CloseHandle(m_hObjThread);
        m_hObjThread = NULL;

        ::WaitForSingleObject(m_hCaptureThread, INFINITE);
        CloseHandle(m_hCaptureThread);
        m_hCaptureThread = NULL;

        Sleep(10);
    }

    DWORD NewIDX;
    m_hCaptureThread = ::CreateThread(NULL, 0, EngineThread, (LPVOID)this, 0, &NewIDX);

    DWORD NewID;
    m_hObjThread = ::CreateThread(NULL, 0, ObjThreadProc, (LPVOID)this, 0, &NewID);

    m_bExitThread = false;
}

bool DrawObjPic::IsRectIntersection(CRect rt1, CRect rt2)
{
    int  iTopX, iTopY, iDownX, iDownY;
    bool  bIsIntersection = false;

    iTopX = max(rt1.left, rt2.left);
    iTopY = max(rt1.top, rt2.top);
    iDownX = min(rt1.right, rt2.right);
    iDownY = min(rt1.bottom, rt2.bottom);

    if ((iTopX < iDownX ) &&( iTopY < iDownY))
    {
        bIsIntersection = true; 
        //rect3->x = iTopX;
        //rect3->y = iTopY;
        //rect3->w = iDownX- iTopX +1;
        //rect3->h = iDownY - iTopY +1;
    }
    else
    {
        bIsIntersection = false;   
        //rect3  = NULL;
    }

    return bIsIntersection;
}

void DrawObjPic::ClearVariableState(void)
{
    ClearObjectInfo();

    memset(m_ObjModifyPt, 0, 4 * sizeof(CvPoint));
    memset(m_iObjectLabelMap, -1, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));

    m_ObjLTPt = cvPoint(0, 0);
    m_ObjRBPt = cvPoint(0, 0);
    m_ObjPos = cvPoint(0, 0);
    m_ObjCurPos = cvPoint(0, 0);

    m_bObjInfoConfirmed = false;
    m_bObjRectDrawed = false;
    m_bOnLButtonUp = false;
    m_bObjectInfoSaved = false;
}

void DrawObjPic::ClearObjectInfo(void)
{
    memset(&stObjectInfo, 0, sizeof(ObjectInfo));
    memset(stObjectInfo.aObjectAreaLabelMap, -1, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));
    stObjectInfo.iObjectIndex = -1;
	stObjectInfo.iObjectPartIndex = -1;
    stObjectInfo.ObjectColor = OBJ_Unkown;
    stObjectInfo.iObjectAngleX = -1;
    stObjectInfo.iObjectAngleY = -1;
}

void DrawObjPic::DrawObjectRectOrGrid(IplImage *pShowImage)
{
	CPoint pt = CPoint(m_CurCursorPos.x, m_CurCursorPos.y); 
	
	for (int i = 0; i < 3; i++)
	{
		cvLine(pShowImage, m_ObjModifyPt[i], m_ObjModifyPt[i + 1], GREEN, 1, 8, 0);
		if (i == 0)
		{
			cvLine(pShowImage, m_ObjModifyPt[0], m_ObjModifyPt[3], GREEN, 1, 8, 0);
		}
	}

	EnterCriticalSection(&m_PlayCriticalSection);

	int iDrawType;
	if (m_iObjectSel != -1 && !m_vObjInfo.empty())
	{
		iDrawType = m_vObjInfo[m_iObjectSel].iDrawType;
	}
	else
	{
		iDrawType = m_iDrawStyle;
	}

	LeaveCriticalSection(&m_PlayCriticalSection);

	stObjectInfo.iDrawType = iDrawType;

	if (iDrawType == 0 && m_iStepX != 0 && m_iStepY != 0)
	{
		for (int x = m_ObjLTPt.x; x <= m_ObjRBPt.x; x += m_iStepX)
		{
			//cvLine(img, cvPoint(x, m_ObjLTPt.y), cvPoint(x, m_ObjRBPt.y), CYAN, 1, 8, 0);
			cvSetImageROI(pShowImage, cvRect(x, m_ObjLTPt.y, 1, m_ObjRBPt.y - m_ObjLTPt.y));
			cvAddS(pShowImage, cvScalar(0, 0, 100), pShowImage, NULL);
			cvResetImageROI(pShowImage);
		}

		for (int y = m_ObjLTPt.y; y <= m_ObjRBPt.y; y += m_iStepY)
		{
			//cvLine(img, cvPoint(m_ObjLTPt.x, y), cvPoint(m_ObjRBPt.x, y), CYAN, 1, 8, 0);
			cvSetImageROI(pShowImage, cvRect(m_ObjLTPt.x, y, m_ObjRBPt.x - m_ObjLTPt.x, 1));
			cvAddS(pShowImage, cvScalar(0, 0, 100), pShowImage, NULL);
			cvResetImageROI(pShowImage);
		}

		CRect MouseRt = CRect(min(m_ObjPos.x, m_ObjCurPos.x), min(m_ObjPos.y, m_ObjCurPos.y), 
			max(m_ObjPos.x, m_ObjCurPos.x), max(m_ObjPos.y, m_ObjCurPos.y));

		int iBlockCnt = 0;
		for (int y = m_ObjLTPt.y; y < m_ObjRBPt.y; y += m_iStepY)
		{
			for (int x = m_ObjLTPt.x; x < m_ObjRBPt.x; x += m_iStepX)
			{
				CRect rt = CRect(x, y, x + m_iStepX, y + m_iStepY);

				cvSetImageROI(pShowImage, cvRect(x, y, m_iStepX, m_iStepY));

				if (m_iObjectLabelMap[iBlockCnt] == 0)
				{
					cvAddS(pShowImage, cvScalar(-100, 0, -100), pShowImage, NULL);
				}
				else if (m_iObjectLabelMap[iBlockCnt] == 1)
				{
					cvAddS(pShowImage, cvScalar(0, -100, 0), pShowImage, NULL);
				}

				cvResetImageROI(pShowImage);

				if ((IsRectIntersection(rt, MouseRt) || PtInRect(&rt, pt)) && m_bOnLabelLBtnDown && (m_bLabelBGArea || m_bLabelOcclution))
				{                        
					if (m_bLabelBGArea)
					{
						m_iObjectLabelMap[iBlockCnt] = 0;
					}
					else if (m_bLabelOcclution)
					{
						m_iObjectLabelMap[iBlockCnt] = 1;
					}

					if (m_bModifyObjLabel)
					{
						m_iObjectLabelMap[iBlockCnt] = -1;
					}
				}

				iBlockCnt++;
			}
		}
	}

	if (m_iObjectSel != -1 && !m_vObjInfo.empty())
	{
		memcpy(m_vObjInfo[m_iObjectSel].aObjectAreaLabelMap, m_iObjectLabelMap, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));
	}
}