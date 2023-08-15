// ObjInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ObjAnno.h"
#include "ObjInfoDlg.h"
#include "DrawObjPic.h"

extern DrawObjPic g_DrawObjPic;
CObjectAnnoDlg *m_pObjAnnoDlg;

// CObjInfoDlg dialog

IMPLEMENT_DYNAMIC(CObjInfoDlg, CDialog)

CObjInfoDlg::CObjInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CObjInfoDlg::IDD, pParent)
{
	m_pObjInfoImage = cvCreateImage(cvSize(2 * 400, 300), IPL_DEPTH_8U, 3);

	m_iObjectAngleX = 0;
	m_iObjectAngleY = 0;
}

CObjInfoDlg::~CObjInfoDlg()
{
	cvReleaseImage(&m_pObjInfoImage);
	m_pObjAnnoDlg->m_bShowObjInfoDlg = FALSE;
	//g_DrawObjPic.m_iObjectSel = -1;
}

void CObjInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ObjectTypeSel, m_ComboCtrl_ObjectTypeSel);
	DDX_Control(pDX, IDC_COMBO_ObjectColorSel, m_ComboCtrl_ObjectColorSel);
	DDX_Control(pDX, IDC_EDIT_LPContext, m_Edit_LPContext);
	DDX_Control(pDX, IDC_COMBO_ObjectPartTypeSel, m_ComboCtrl_ObjectPartTypeSel);
}


BEGIN_MESSAGE_MAP(CObjInfoDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_PrevObj, &CObjInfoDlg::OnBnClickedButtonPrevobj)
	ON_BN_CLICKED(IDC_BUTTON_NextObj, &CObjInfoDlg::OnBnClickedButtonNextobj)
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectTypeSel, &CObjInfoDlg::OnCbnSelchangeComboObjecttypesel)
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectColorSel, &CObjInfoDlg::OnCbnSelchangeComboObjectcolorsel)
	ON_BN_CLICKED(IDOK, &CObjInfoDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectPartTypeSel, &CObjInfoDlg::OnCbnSelchangeComboObjectparttypesel)
END_MESSAGE_MAP()


// CObjInfoDlg message handlers

BOOL CObjInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	for (int i = 0; i < g_DrawObjPic.stObjectConfig.iObjectNum; i++)
	{
		m_ComboCtrl_ObjectTypeSel.InsertString(i, g_DrawObjPic.stObjectConfig.aObjectType[i]);
	}

	for (int i = 0; i < g_DrawObjPic.stObjectConfig.iObjectPartNum; i++)
	{
		m_ComboCtrl_ObjectPartTypeSel.InsertString(i, g_DrawObjPic.stObjectConfig.aObjectPartType[i]);
	}

	m_ComboCtrl_ObjectPartTypeSel.InsertString(g_DrawObjPic.stObjectConfig.iObjectPartNum, _T("不选"));

	for (int i = 0; i < g_DrawObjPic.stObjectConfig.iObjectColorNum; i++)
	{
		m_ComboCtrl_ObjectColorSel.InsertString(i, g_DrawObjPic.stObjectConfig.aObjectColor[i]);
	}

	m_ComboCtrl_ObjectColorSel.InsertString(g_DrawObjPic.stObjectConfig.iObjectColorNum, _T("不选"));

	if (!m_pObjAnnoDlg)
	{
		m_pObjAnnoDlg = (CObjectAnnoDlg*)m_pObjAnnoWnd;
	}

	m_pObjAnnoDlg->m_bShowObjInfoDlg = TRUE;

	m_iObjectAngleX = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectAngleX;
	m_iObjectAngleY = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectAngleY;
	m_iObjectTypeSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectIndex;
	m_iObjectPartSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectPartIndex;
	m_iObjectColorSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].ObjectColor;
		
	ShowObjectInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CObjInfoDlg::DrawPic2HDC(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC  = pDC->GetSafeHdc();

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img);
	cimg.DrawToHDC(hDC,&rect);
	ReleaseDC(pDC);
}

void CObjInfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	DrawPic2HDC(m_pObjInfoImage, IDC_STATIC_ObjInfo);

	Invalidate(FALSE);
}

void CObjInfoDlg::OnBnClickedButtonPrevobj()
{
	// TODO: Add your control notification handler code here
	if (m_iObjectSel >= 1)
	{
		m_iObjectSel--;
	}

	m_iObjectAngleX = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectAngleX;
	m_iObjectAngleY = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectAngleY;
	m_iObjectTypeSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectIndex;
	m_iObjectPartSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectPartIndex;
	m_iObjectColorSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].ObjectColor;

	ShowObjectInfo();
}

void CObjInfoDlg::OnBnClickedButtonNextobj()
{
	// TODO: Add your control notification handler code here
	if (m_iObjectSel < g_DrawObjPic.m_nObjAnnoNum - 1)
	{
		m_iObjectSel++;
	}

	m_iObjectAngleX = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectAngleX;
	m_iObjectAngleY = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectAngleY;
	m_iObjectTypeSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectIndex;
	m_iObjectPartSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].iObjectPartIndex;
	m_iObjectColorSel = g_DrawObjPic.m_vObjInfo[m_iObjectSel].ObjectColor;

	ShowObjectInfo();
}

void CObjInfoDlg::OnCbnSelchangeComboObjecttypesel()
{
	// TODO: Add your control notification handler code here
	m_iObjectTypeSel = ((CComboBox*)GetDlgItem(IDC_COMBO_ObjectTypeSel))->GetCurSel();

	ShowObjectInfo();
}

void CObjInfoDlg::ShowObjectInfo()
{
	ObjectInfo stTempObjectInfo = g_DrawObjPic.m_vObjInfo[m_iObjectSel];

	m_ComboCtrl_ObjectTypeSel.SetCurSel(m_iObjectTypeSel);
	m_ComboCtrl_ObjectPartTypeSel.SetCurSel(m_iObjectPartSel);
	m_ComboCtrl_ObjectColorSel.SetCurSel(m_iObjectColorSel);
	m_Edit_LPContext.SetWindowText(stTempObjectInfo.aObjectContext);

	if (m_iObjectPartSel == 2)
	{
		m_Edit_LPContext.EnableWindow(TRUE);
	}
	else
	{
		m_Edit_LPContext.EndModalLoop(FALSE);
	}


	strcpy(stTempObjectInfo.aObjectType, g_DrawObjPic.stObjectConfig.aObjectType[m_iObjectTypeSel]);

	if (stTempObjectInfo.bHaveContext)
	{
		m_Edit_LPContext.SetWindowText(stTempObjectInfo.aObjectContext);
	}
	
	int iRectWidth = (g_DrawObjPic.m_ImageRect.Width() / 4) * 4;
	int iRectheight = g_DrawObjPic.m_ImageRect.Height();
	int iObjectRectW, iObjectRectH, iResizeW, iResizeH;
	int iMinX = 1000000;
	int iMinY = 1000000;
	int iMaxX = -1000000;
	int iMaxY = -1000000;
	if (stTempObjectInfo.iDrawType == 0)
	{
		iObjectRectW = stTempObjectInfo.stObjectRectPt[2].x - stTempObjectInfo.stObjectRectPt[0].x;
		iObjectRectH = stTempObjectInfo.stObjectRectPt[2].y - stTempObjectInfo.stObjectRectPt[0].y;
	}
	else if (stTempObjectInfo.iDrawType == 1)
	{
		for (int i = 0; i < 4; i++)
		{
			iMinX = std::min(iMinX, stTempObjectInfo.stObjectRectPt[i].x);
			iMinY = std::min(iMinY, stTempObjectInfo.stObjectRectPt[i].y);
			iMaxX = std::max(iMaxX, stTempObjectInfo.stObjectRectPt[i].x);
			iMaxY = std::max(iMaxY, stTempObjectInfo.stObjectRectPt[i].y);
		}

		iObjectRectW = iMaxX - iMinX;
		iObjectRectH = iMaxY - iMinY;
	}

	float ftp = 400.f / 300.f;
	float fRatio = (float)iObjectRectW / (float)iObjectRectH;

	if (fRatio > ftp)
	{
		iResizeW = 400;
		iResizeH = iResizeW / fRatio;
	}
	else
	{
		iResizeH = 300;
		iResizeW = iResizeH * fRatio;
	}
	
	IplImage *pSrcImage   = cvCreateImageHeader(cvSize(iRectWidth, iRectheight), IPL_DEPTH_8U, 3);
	IplImage *pCheckImage = cvCreateImage(cvSize(400 * 2, 300), IPL_DEPTH_8U, 3);
	cvZero(pCheckImage);

	pSrcImage->imageData = g_DrawObjPic.m_ImageShow;

	{
		char ModelName[20];
		sprintf(ModelName, "%d_%d.png", m_iObjectAngleY, m_iObjectAngleX);

		std::string strModelPath = std::string(g_DrawObjPic.m_aExePath) + std::string("/VehicleModel/")
			+ std::string(stTempObjectInfo.aObjectType) + std::string("/") + ModelName;

		IplImage *pModelImage = cvLoadImage(strModelPath.c_str(), CV_LOAD_IMAGE_COLOR);

		if (pModelImage != NULL)
		{
			cvSetImageROI(pCheckImage, cvRect(400, 0, 400, 300));

			cvResize(pModelImage, pCheckImage, CV_INTER_LINEAR);

			cvResetImageROI(pCheckImage);
		}

		cvSetImageROI(pCheckImage, cvRect((400 - iResizeW) / 2, (300 - iResizeH) / 2, iResizeW, iResizeH));

		if (stTempObjectInfo.iDrawType == 0)
		{
			cvSetImageROI(pSrcImage, cvRect(stTempObjectInfo.stObjectRectPt[0].x, stTempObjectInfo.stObjectRectPt[0].y, iObjectRectW, iObjectRectH));
		}
		else if (stTempObjectInfo.iDrawType == 1)
		{
			cvSetImageROI(pSrcImage, cvRect(iMinX, iMinY, iObjectRectW, iObjectRectH));
		}
		
		cvResize(pSrcImage, pCheckImage, CV_INTER_LINEAR);

		cvResetImageROI(pCheckImage);
		cvResetImageROI(pSrcImage);

		cvReleaseImage(&pModelImage);
	}

	cvRectangle(pCheckImage, cvPoint(0, 0), cvPoint(pCheckImage->width, pCheckImage->height),
		BLACK, 2, 8, 0);

	cvCopy(pCheckImage, m_pObjInfoImage, NULL);	

	cvReleaseImageHeader(&pSrcImage);
	cvReleaseImage(&pCheckImage);

	Invalidate(FALSE);
	UpdateWindow();
}
void CObjInfoDlg::OnCbnSelchangeComboObjectcolorsel()
{
	// TODO: Add your control notification handler code here
	m_iObjectColorSel = m_ComboCtrl_ObjectColorSel.GetCurSel();

	if (m_iObjectColorSel == g_DrawObjPic.stObjectConfig.iObjectColorNum)
	{
		m_iObjectColorSel = -1;
		m_ComboCtrl_ObjectColorSel.SetCurSel(m_iObjectColorSel);
	}
}

void CObjInfoDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	ObjectInfo stTempObjectInfo = g_DrawObjPic.m_vObjInfo[m_iObjectSel];

	CString strBuf;
	GetDlgItem(IDC_EDIT_LPContext)->GetWindowText(strBuf);

	if (m_iObjectPartSel == 2)
	{
		strcpy(stTempObjectInfo.aObjectContext, strBuf.GetBuffer());

		if (!strBuf.IsEmpty())
		{
			stTempObjectInfo.bHaveContext = TRUE;
		}
		else
		{
			stTempObjectInfo.bHaveContext = FALSE;
		}
	}

	if (m_iObjectPartSel == -1)
	{
		memset(stTempObjectInfo.aObjectPartType, 0, _MAX_PATH * sizeof(char));
		stTempObjectInfo.bObjectPart = FALSE;
	}

	stTempObjectInfo.iObjectIndex = m_iObjectTypeSel;
	strcpy(stTempObjectInfo.aObjectType, g_DrawObjPic.stObjectConfig.aObjectType[m_iObjectTypeSel]);
	stTempObjectInfo.iObjectPartIndex = m_iObjectPartSel;
	if (stTempObjectInfo.iObjectPartIndex != -1)
	{
		stTempObjectInfo.bObjectPart = TRUE;
		strcpy(stTempObjectInfo.aObjectPartType, g_DrawObjPic.stObjectConfig.aObjectPartType[m_iObjectPartSel]);
	}
	stTempObjectInfo.ObjectColor = (enObjectColor)m_iObjectColorSel;

	if (m_iObjectPartSel == 2)
	{
		stTempObjectInfo.iObjectAngleX = -1;
		stTempObjectInfo.iObjectAngleY = -1;
	}
	else
	{
		stTempObjectInfo.iObjectAngleX = m_iObjectAngleX;
		stTempObjectInfo.iObjectAngleY = m_iObjectAngleY;
	}

	g_DrawObjPic.m_vObjInfo[m_iObjectSel] = stTempObjectInfo;

	m_pObjAnnoDlg->m_ListCtrl_ObjectInfo.DeleteAllItems();

	for (int i = 0; i < g_DrawObjPic.m_nObjAnnoNum; i++)
	{
		stTempObjectInfo = g_DrawObjPic.m_vObjInfo[i];

		char cBuf[20];
		sprintf(cBuf, "%d", i + 1);
		m_pObjAnnoDlg->m_ListCtrl_ObjectInfo.InsertItem(i, cBuf);
		m_pObjAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(i, 1, stTempObjectInfo.aObjectType);
		m_pObjAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(i, 2, stTempObjectInfo.aObjectPartType);
		m_pObjAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(i, 3, g_DrawObjPic.stObjectConfig.aObjectColor[stTempObjectInfo.ObjectColor]);
		m_pObjAnnoDlg->m_ListCtrl_ObjectInfo.SetItemText(i, 4, stTempObjectInfo.aObjectContext);
	}
}

BOOL CObjInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_iObjectPartSel == 2)
	{
		return FALSE;
	}

	if(pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case 'W':
			m_iObjectAngleY -= 10;
			if (m_iObjectAngleY < 0)
			{
				m_iObjectAngleY = 60;
			}

			if (m_iObjectAngleX == -1)
			{
				m_iObjectAngleX = 0;
			}

			ShowObjectInfo();
			break;
		case 'S':
			m_iObjectAngleY += 10;
			if (m_iObjectAngleY > 60)
			{
				m_iObjectAngleY = 0;
			}

			if (m_iObjectAngleX == -1)
			{
				m_iObjectAngleX = 0;
			}

			ShowObjectInfo();
			break;
		case 'A':
			m_iObjectAngleX -=  15;
			if (m_iObjectAngleX < 0)
			{
				m_iObjectAngleX = 345;
			}

			if (m_iObjectAngleY == -1)
			{
				m_iObjectAngleY = 0;
			}

			ShowObjectInfo();
			break;
		case 'D':
			m_iObjectAngleX += 15;
			if (m_iObjectAngleX > 345)
			{
				m_iObjectAngleX = 0;
			}

			if (m_iObjectAngleY == -1)
			{
				m_iObjectAngleY = 0;
			}

			ShowObjectInfo();
			break;
		default:
			break;
		}

		pMsg->wParam = 0;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CObjInfoDlg::OnCbnSelchangeComboObjectparttypesel()
{
	// TODO: Add your control notification handler code here
	m_iObjectPartSel = ((CComboBox*)GetDlgItem(IDC_COMBO_ObjectPartTypeSel))->GetCurSel();

	if (m_iObjectPartSel == g_DrawObjPic.stObjectConfig.iObjectPartNum)
	{
		m_iObjectPartSel = -1;
		m_ComboCtrl_ObjectPartTypeSel.SetCurSel(m_iObjectPartSel);
	}

	if (m_iObjectPartSel == 2)//车牌
	{
		GetDlgItem(IDC_EDIT_LPContext)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_LPContext)->EnableWindow(FALSE);
	}

	ShowObjectInfo();
}
