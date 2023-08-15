// CarCaptruePictureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ObjAnno.h"
#include "ObjAnnoDlg.h"
#include "DrawObjPic.h"
#include "ObjInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern DrawObjPic g_DrawObjPic;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CObjectAnnoDlg 对话框
CObjectAnnoDlg::CObjectAnnoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectAnnoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CObjectAnnoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ObjectInfo, m_ListCtrl_ObjectInfo);
	DDX_Control(pDX, IDC_COMBO_ObjectType, m_ComboCtrl_ObjectType);
	DDX_Control(pDX, IDC_SLIDER_FileProgress, m_SliderCtrl_FileProgress);
	DDX_Control(pDX, IDC_STATIC_FileProgress, m_Static_FileProcess);
	DDX_Control(pDX, IDC_Static_ImageShow, m_Static_ImageShow);
	DDX_Control(pDX, IDC_COMBO_DrawStyle, m_ComboCtrl_DrawStyle);
	DDX_Control(pDX, IDC_COMBO_ObjectColor, m_ComboCtrl_ObjectColor);
	DDX_Control(pDX, IDC_EDIT_LPID, m_Edit_ObjectContext);
	DDX_Control(pDX, IDC_COMBO_ObjectPartType, m_ComboCtrl_ObjectPartType);
	DDX_Control(pDX, IDC_RADIO_LABEL_BGArea, m_RadioCtrl_LabelBGArea);
	DDX_Control(pDX, IDC_Combo_VehicleLogo_Type, m_ComboBox_VehicleLogo_Type);
}

BEGIN_MESSAGE_MAP(CObjectAnnoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Btn_ImportImage, OnBnClickedBtnImportImage)
	ON_WM_LBUTTONDOWN(IDC_PIC,OnLButtonDown)
	ON_WM_LBUTTONUP(IDC_PIC,OnLButtonDownUp)
	ON_WM_MOUSEMOVE(IDC_PIC,OnMouseMove)
	ON_BN_CLICKED(IDC_Btn_ReAnno, OnBnClickedBtnReAnno)
	ON_BN_CLICKED(IDC_Btn_PrevFile, OnBnClickedBtnPreFile)
	ON_BN_CLICKED(IDC_Btn_Undo, OnBnClickedBtnUndo)
	ON_BN_CLICKED(IDC_Btn_SaveObjInfo, OnBnClickedBtnSaveObjInfo)
	ON_BN_CLICKED(IDC_Btn_ImportVideo, OnBnClickedBtnImportVideo)
	ON_MESSAGE(WM_MSG_CAR, OnCWndDealMsg)
	ON_BN_CLICKED(IDC_Btn_Play, OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_Btn_PlayAccelerate, OnBnClickedBtnPlayAccelerate)
	ON_BN_CLICKED(IDC_Btn_SelectPlayROI, OnBnClickedBtnSelectPlayROI)
	ON_BN_CLICKED(IDC_Btn_OriVideo, OnBnClickedBtnOriVideo)
	ON_BN_CLICKED(IDC_Btn_PlayDecelerate, OnBnClickedBtnPlayDecelerate)
	ON_WM_DESTROY()

	ON_WM_KEYDOWN()
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectType, OnCbnSelchangeComboObjectType)
	ON_BN_CLICKED(IDC_Btn_NextFile, OnBnClickedBtnNextFile)
	ON_BN_CLICKED(IDC_Btn_PlayForward, OnBnClickedBtnPlayForward)
	ON_BN_CLICKED(IDC_Btn_PlayBackward, OnBnClickedBtnPlayBackward)
	ON_WM_SIZE()
    ON_BN_CLICKED(IDC_RADIO_LABEL_BGArea, &CObjectAnnoDlg::OnBnClickedRadioLabelBgarea)
    ON_BN_CLICKED(IDC_RADIO_LABEL_OcclutionArea, &CObjectAnnoDlg::OnBnClickedRadioLabelOcclutionarea)
    ON_CBN_SELCHANGE(IDC_COMBO_ModeSelect, &CObjectAnnoDlg::OnCbnSelchangeComboModeselect)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_ObjectInfo, &CObjectAnnoDlg::OnNMDblclkListObjectinfo)
    ON_WM_HSCROLL()
    ON_WM_SIZING()
    ON_CBN_SELCHANGE(IDC_COMBO_DrawStyle, &CObjectAnnoDlg::OnCbnSelchangeComboDrawstyle)
    ON_NOTIFY(NM_CLICK, IDC_LIST_ObjectInfo, &CObjectAnnoDlg::OnNMClickListObjectinfo)
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectPartType, &CObjectAnnoDlg::OnCbnSelchangeComboObjectparttype)
	ON_CBN_SELCHANGE(IDC_COMBO_ObjectColor, &CObjectAnnoDlg::OnCbnSelchangeComboObjectcolor)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CObjectAnnoDlg)
    EASYSIZE(IDC_STATIC_Group, ES_BORDER, ES_BORDER, IDC_Static_ImageShow, ES_BORDER, 0)
    //EASYSIZE(IDC_STATIC_Tips, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_BORDER, 0)
    EASYSIZE(IDC_Static_ImageShow, IDC_STATIC_Group, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP
// CObjectAnnoDlg 消息处理程序
BOOL CObjectAnnoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_bShowObjInfoDlg = FALSE;

    TCHAR sAppPath[_MAX_PATH], sDrive[_MAX_DRIVE], sDir[_MAX_DIR];
    GetModuleFileName(NULL, sAppPath, _MAX_PATH);
    _splitpath(sAppPath, sDrive, sDir, NULL, NULL);  //主程序所在的目录

    sprintf(g_DrawObjPic.m_aExePath, "%s%s", sDrive, sDir);

	g_DrawObjPic.m_hWnd        = this->m_hWnd;
    g_DrawObjPic.m_pObjAnnoWnd = this;

    ReadConfigParams();

    for (int i = 0; i < g_DrawObjPic.stObjectConfig.iObjectNum; i++)
    {
        m_ComboCtrl_ObjectType.InsertString(i, g_DrawObjPic.stObjectConfig.aObjectType[i]);
    }

	for (int i = 0; i < g_DrawObjPic.stObjectConfig.iObjectPartNum; i++)
	{
		m_ComboCtrl_ObjectPartType.InsertString(i, g_DrawObjPic.stObjectConfig.aObjectPartType[i]);
	}

	m_ComboCtrl_ObjectPartType.InsertString(g_DrawObjPic.stObjectConfig.iObjectPartNum, _T("不选"));

    for (int i = 0; i < g_DrawObjPic.stObjectConfig.iObjectColorNum; i++)
    {
        m_ComboCtrl_ObjectColor.InsertString(i, g_DrawObjPic.stObjectConfig.aObjectColor[i]);
    }

	m_ComboCtrl_ObjectColor.InsertString(g_DrawObjPic.stObjectConfig.iObjectColorNum, _T("不选"));

    ((CComboBox*)GetDlgItem(IDC_COMBO_ModeSelect))->InsertString(0, _T("标注"));
    ((CComboBox*)GetDlgItem(IDC_COMBO_ModeSelect))->InsertString(1, _T("核查"));

    ((CComboBox*)GetDlgItem(IDC_COMBO_DrawStyle))->InsertString(0, _T("栅格"));
    ((CComboBox*)GetDlgItem(IDC_COMBO_DrawStyle))->InsertString(1, _T("平行四边形"));

    m_ListCtrl_ObjectInfo.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE);
    m_ListCtrl_ObjectInfo.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 25);
    m_ListCtrl_ObjectInfo.InsertColumn(1, _T("目标类别"), LVCFMT_LEFT, 60);
	m_ListCtrl_ObjectInfo.InsertColumn(2, _T("部件类型"), LVCFMT_LEFT, 60);
    m_ListCtrl_ObjectInfo.InsertColumn(3, _T("目标颜色"), LVCFMT_LEFT, 60);
	m_ListCtrl_ObjectInfo.InsertColumn(4, _T("备注"), LVCFMT_LEFT, 70);

    m_ToolTipCtrl.Create(this);
    m_ToolTipCtrl.Activate(TRUE);
    //m_ToolTipCtrl.SetTipBkColor(RGB(0, 255, 255));
    //m_ToolTipCtrl.SetTipTextColor(RGB(255, 0, 0));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_ReAnno), _T("重新标注(Ctrl+R)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_Undo), _T("撤销(Ctrl+Z)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_SaveObjInfo), _T("保存标注(Ctrl+S)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_PrevFile), _T("上一个(Page Up)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_NextFile), _T("下一个(Page Down)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_Play), _T("播放/暂停(Space)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_PlayAccelerate), _T("加速(Ctrl+↑)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_PlayDecelerate), _T("减速(Ctrl+↓)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_PlayForward), _T("前进(Ctrl+→)"));
    m_ToolTipCtrl.AddTool(GetDlgItem(IDC_Btn_PlayBackward), _T("后退(Ctrl+←)"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_RADIO_LABEL_BGArea), _T("可以用Tab键来进行切换"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_RADIO_LABEL_OcclutionArea), _T("可以用Tab键来进行切换"));

    CString strBuf;
	strBuf.Format("使用说明:\n Ctrl: 修改标注框(平行四边形)\n Shift: 修改标注区域\n Q:取消目标选定状态\n F: 目标标注完成确定键\n w/s/a/d: 显示目标各个角度模型");
    GetDlgItem(IDC_STATIC_Tips)->SetWindowText(strBuf);

    //ModifyStyleEx(0, WS_EX_APPWINDOW|WS_EX_CONTROLPARENT, SWP_NOSIZE);
    //ModifyStyle(WS_BORDER|WS_DLGFRAME|WS_THICKFRAME, WS_POPUP, SWP_NOSIZE);
	
    CRect rt(0, 0, GetSystemMetrics(SM_CXSCREEN)-80, GetSystemMetrics(SM_CYSCREEN)-80);

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0); 

    MoveWindow(&rt, FALSE);

    INIT_EASYSIZE;

    g_DrawObjPic.UpdateImageRect(); //初始化

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CObjectAnnoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CObjectAnnoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CObjectAnnoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CObjectAnnoDlg::OnBnClickedBtnImportImage()
{
	// TODO: 在此添加控件通知处理程序代码
    int iSel1 = ((CComboBox*)GetDlgItem(IDC_COMBO_ModeSelect))->GetCurSel();
    if (-1 == iSel1)
    {
        AfxMessageBox(_T("请先选择标注模式"));
        return;
    }

    int iSel2 = ((CComboBox*)GetDlgItem(IDC_COMBO_DrawStyle))->GetCurSel();

    if (-1 == iSel2 && iSel1 != 1)
    {
        AfxMessageBox(_T("请先选择标注样式"));
        return;
    }

	bool Working = g_DrawObjPic.LoadImageScource(1);

	if(Working)
	{
        GetDlgItem(IDC_COMBO_ModeSelect)->EnableWindow(FALSE);
        //GetDlgItem(IDC_COMBO_DrawStyle)->EnableWindow(FALSE);       
        GetDlgItem(IDC_Btn_ImportVideo)->EnableWindow(FALSE);
		GetDlgItem(IDC_Btn_NextFile)->EnableWindow(TRUE);
		GetDlgItem(IDC_Btn_PrevFile)->EnableWindow(TRUE);
	}
}

void CObjectAnnoDlg::OnBnClickedBtnImportVideo()
{
    // TODO: 在此添加控件通知处理程序代码
    int iSel = ((CComboBox*)GetDlgItem(IDC_COMBO_ModeSelect))->GetCurSel();
    if (-1 == iSel)
    {
        AfxMessageBox(_T("请先选择标注模式"));
        return;
    }

    iSel = ((CComboBox*)GetDlgItem(IDC_COMBO_DrawStyle))->GetCurSel();

    if (-1 == iSel)
    {
        AfxMessageBox(_T("请先选择标注样式"));
        return;
    }

    bool Working = g_DrawObjPic.LoadImageScource(2);

    if(Working)
    {
        GetDlgItem(IDC_Btn_Play)->SetWindowText(_T("暂停"));
        GetDlgItem(IDC_COMBO_ModeSelect)->EnableWindow(FALSE);
        //GetDlgItem(IDC_COMBO_DrawStyle)->EnableWindow(FALSE);
        GetDlgItem(IDC_Btn_ImportImage)->EnableWindow(FALSE);
        GetDlgItem(IDC_Btn_Play)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_SelectPlayROI)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_OriVideo)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_PlayAccelerate)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_PlayDecelerate)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_PlayForward)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_PlayBackward)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_NextFile)->EnableWindow(TRUE);
        GetDlgItem(IDC_Btn_PrevFile)->EnableWindow(TRUE);
    }
}

void CObjectAnnoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	g_DrawObjPic.DrawOnLButtonDown(nFlags, point);
}

void CObjectAnnoDlg::OnLButtonUp(UINT nFlags,CPoint point)
{
	g_DrawObjPic.DrawOnLButtonUp(nFlags, point);
}

void CObjectAnnoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	g_DrawObjPic.DrawOnMouseMove(nFlags, point);
}

void CObjectAnnoDlg::OnBnClickedBtnReAnno()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.ReDrawPic();
}

void CObjectAnnoDlg::OnBnClickedBtnPreFile()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.LoadPrevFile();
}

void CObjectAnnoDlg::OnBnClickedBtnUndo()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.UndoDrawObj();
}

void CObjectAnnoDlg::OnBnClickedBtnSaveObjInfo()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.WriteObjectAnnoInfo();
    g_DrawObjPic.m_bObjectInfoSaved = true;
}

LRESULT CObjectAnnoDlg::OnCWndDealMsg(WPARAM wParam,LPARAM lParam)
{
	char CPicWidth[50];
	sprintf(CPicWidth, "W:%d", g_DrawObjPic.m_OriImageWidth);

	char CPicHeight[50];
	sprintf(CPicHeight, "H:%d", g_DrawObjPic.m_OriImageHeight);

	char CPicInitx[20];
	sprintf(CPicInitx, "x:%f", g_DrawObjPic.m_fWidthScale);

	char CPixInity[20];
	sprintf(CPixInity, "y:%f", g_DrawObjPic.m_fHeightScale);

	char CAviFrameAllCount[20];
	sprintf(CAviFrameAllCount, "%d", g_DrawObjPic.m_nVideoFrameNum);

	char CAviFrameCurrentCount[20];
	sprintf(CAviFrameCurrentCount, "%d", g_DrawObjPic.m_nVideoFrameIdx);

	return 1;
}

void CObjectAnnoDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!g_DrawObjPic.m_bPause)
	{
		GetDlgItem(IDC_Btn_Play)->SetWindowText("播放");
		GetDlgItem(IDC_Btn_SelectPlayROI)->EnableWindow(false);
		GetDlgItem(IDC_Btn_PlayAccelerate)->EnableWindow(false);
		GetDlgItem(IDC_Btn_PlayDecelerate)->EnableWindow(false);
		GetDlgItem(IDC_Btn_PlayForward)->EnableWindow(false);
		GetDlgItem(IDC_Btn_PlayBackward)->EnableWindow(false);

		g_DrawObjPic.StopPlay();
	}
	else
	{
		GetDlgItem(IDC_Btn_Play)->SetWindowText("暂停");
		GetDlgItem(IDC_Btn_SelectPlayROI)->EnableWindow(true);
		GetDlgItem(IDC_Btn_PlayAccelerate)->EnableWindow(true);
		GetDlgItem(IDC_Btn_PlayDecelerate)->EnableWindow(true);
		GetDlgItem(IDC_Btn_PlayForward)->EnableWindow(true);
		GetDlgItem(IDC_Btn_PlayBackward)->EnableWindow(true);

		g_DrawObjPic.RealPlay();
	}
}

void CObjectAnnoDlg::OnBnClickedBtnPlayAccelerate()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.PlayAccelerate();
	GetDlgItem(IDC_Btn_Play)->SetFocus();
}

void CObjectAnnoDlg::OnBnClickedBtnSelectPlayROI()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(false);
	g_DrawObjPic.RealPlayRect();
}

void CObjectAnnoDlg::OnBnClickedBtnOriVideo()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.ReRealPlay();
}

void CObjectAnnoDlg::OnBnClickedBtnPlayDecelerate()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.PlayDecelerate();
	GetDlgItem(IDC_Btn_Play)->SetFocus();
}

void CObjectAnnoDlg::OnDestroy()
{
	g_DrawObjPic.ExitThread();

	CDialog::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}

BOOL CObjectAnnoDlg::PreTranslateMessage(MSG *msg)
{
	if (m_bShowObjInfoDlg)
	{
		return FALSE;
	}

	CWnd *pFocus = GetFocus();
	if ( pFocus && pFocus->m_hWnd == m_Edit_ObjectContext.m_hWnd)
	{
		return FALSE;
	}

    int iSel = -1;
	int iObjectSel = -1;
	int iObjectPartSel = -1;
    POSITION pos;
  	if(msg->message == WM_KEYDOWN)
	{
        switch (msg->wParam)
        {
		case VK_TAB:
			msg->wParam = 0;
			if (g_DrawObjPic.m_bLabelBGArea == false)
			{
				g_DrawObjPic.m_bLabelBGArea = true;
				g_DrawObjPic.m_bLabelOcclution = false;
				((CButton*)GetDlgItem(IDC_RADIO_LABEL_BGArea))->SetCheck(TRUE);
				((CButton*)GetDlgItem(IDC_RADIO_LABEL_OcclutionArea))->SetCheck(FALSE);
			}
			else
			{
				g_DrawObjPic.m_bLabelBGArea = false;
				g_DrawObjPic.m_bLabelOcclution = true;
				((CButton*)GetDlgItem(IDC_RADIO_LABEL_BGArea))->SetCheck(FALSE);
				((CButton*)GetDlgItem(IDC_RADIO_LABEL_OcclutionArea))->SetCheck(TRUE);
			}

			break;
		case 'Q':
			msg->wParam = 0;
			if (g_DrawObjPic.m_iObjectSel != -1)
			{
				g_DrawObjPic.m_iObjectSel = -1;
			}
			else
			{
				return FALSE;
			}
			
			g_DrawObjPic.ClearVariableState();
			break;
        case VK_SPACE:
            msg->wParam = 0;
			if (g_DrawObjPic.FileType == 2)
			{
				OnBnClickedBtnPlay();
			}
            break;
        case VK_CONTROL:
            if (g_DrawObjPic.m_iDrawStyle == 1)
            {
                g_DrawObjPic.m_bModifyObjRect = true;
                //g_DrawObjPic.m_bObjRectDrawed = false;
            }
            break;
		case VK_ATTN:
        case VK_SHIFT:
            if (g_DrawObjPic.m_iDrawStyle == 0)
            {
                g_DrawObjPic.m_bModifyObjLabel = true;
            }
            break;     
		case VK_RETURN:
			msg->wParam = 0;
			break;
		case 'F'://VK_F5:
            msg->wParam = 0; //消除默认回车键关闭对话框

            if (g_DrawObjPic.m_bObjRectDrawed == false)
            {
                AfxMessageBox("请先标注目标框");
                break;
            }

            iObjectSel = m_ComboCtrl_ObjectType.GetCurSel();
            if (iObjectSel == -1)
            {
                AfxMessageBox("请选选择目标类型");
                break;
            }

			iObjectPartSel = m_ComboCtrl_ObjectPartType.GetCurSel();
            
            if (iObjectPartSel != 2)
            {
				m_Edit_ObjectContext.SetWindowText(_T(""));

                if (g_DrawObjPic.stObjectInfo.iObjectAngleX == -1 ||
                    g_DrawObjPic.stObjectInfo.iObjectAngleY == -1)
                {
                    g_DrawObjPic.ShowObjectModel(g_DrawObjPic.m_iObjectAngleX, g_DrawObjPic.m_iObjectAngleY);
                    break;
                }  
            }

            if (g_DrawObjPic.m_bObjRectDrawed)
            {
                cvDestroyWindow("ModelImage");
                g_DrawObjPic.m_bObjInfoConfirmed = true;
            }
            break;
        case 'W':
            g_DrawObjPic.m_iObjectAngleY -= 10;
            if (g_DrawObjPic.m_iObjectAngleY < 0)
            {
                g_DrawObjPic.m_iObjectAngleY = 60;
            }           

            g_DrawObjPic.ShowObjectModel(g_DrawObjPic.m_iObjectAngleX, g_DrawObjPic.m_iObjectAngleY);
            break;
        case 'S':
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                OnBnClickedBtnSaveObjInfo();
                break;
            }

            g_DrawObjPic.m_iObjectAngleY += 10;
            if (g_DrawObjPic.m_iObjectAngleY > 60)
            {
                g_DrawObjPic.m_iObjectAngleY = 0;
            }

            g_DrawObjPic.ShowObjectModel(g_DrawObjPic.m_iObjectAngleX, g_DrawObjPic.m_iObjectAngleY);
            break;
        case 'A':
            g_DrawObjPic.m_iObjectAngleX -=  15;
            if (g_DrawObjPic.m_iObjectAngleX < 0)
            {
                g_DrawObjPic.m_iObjectAngleX = 345;
            }

            g_DrawObjPic.ShowObjectModel(g_DrawObjPic.m_iObjectAngleX, g_DrawObjPic.m_iObjectAngleY);
            break;
        case 'D':
            g_DrawObjPic.m_iObjectAngleX += 15;
            if (g_DrawObjPic.m_iObjectAngleX > 345)
            {
                g_DrawObjPic.m_iObjectAngleX = 0;
            }

            g_DrawObjPic.ShowObjectModel(g_DrawObjPic.m_iObjectAngleX, g_DrawObjPic.m_iObjectAngleY);
            break;
        case 'Z':
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                OnBnClickedBtnUndo();
            }
            break;
        case 'R':
            if ((GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_SHIFT) & 0x8000))
            {
                OnBnClickedBtnReAnno();
            }
            break;
        case VK_PRIOR:
            OnBnClickedBtnPreFile();
            break;
        case VK_NEXT:
            OnBnClickedBtnNextFile();
            break;
        case VK_UP:
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                OnBnClickedBtnPlayAccelerate();
            }
            msg->wParam = 0;
            break;
        case VK_DOWN:
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                OnBnClickedBtnPlayDecelerate();
            }
            msg->wParam = 0;
            break;
        case VK_LEFT:
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                OnBnClickedBtnPlayBackward();
            }
            msg->wParam = 0;
            break;
        case VK_RIGHT:
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                OnBnClickedBtnPlayForward();
            }
            msg->wParam = 0;
            break;
        case VK_DELETE:
            pos = m_ListCtrl_ObjectInfo.GetFirstSelectedItemPosition();

			EnterCriticalSection(&g_DrawObjPic.m_PlayCriticalSection);
            if (pos)
            {
                while (pos)
                {
                    iSel = m_ListCtrl_ObjectInfo.GetNextSelectedItem(pos);
                    m_ListCtrl_ObjectInfo.DeleteItem(iSel);
                    g_DrawObjPic.m_nObjAnnoNum--;
                    g_DrawObjPic.m_vObjInfo.erase(g_DrawObjPic.m_vObjInfo.begin() + iSel);
					//g_DrawObjPic.m_iObjectSel--;
                }
            }

			g_DrawObjPic.m_iObjectSel = -1;
			g_DrawObjPic.ClearVariableState();
			
			LeaveCriticalSection(&g_DrawObjPic.m_PlayCriticalSection);
			
			m_ListCtrl_ObjectInfo.DeleteAllItems();

			for (int i = 0; i < g_DrawObjPic.m_nObjAnnoNum; i++)
			{
				ObjectInfo stTempObjectInfo = g_DrawObjPic.m_vObjInfo[i];
				char cBuf[20];
				sprintf(cBuf, "%d", i + 1);
				m_ListCtrl_ObjectInfo.InsertItem(i, cBuf);
				m_ListCtrl_ObjectInfo.SetItemText(i, 1, stTempObjectInfo.aObjectType);
				m_ListCtrl_ObjectInfo.SetItemText(i, 2, stTempObjectInfo.aObjectPartType);
				m_ListCtrl_ObjectInfo.SetItemText(i, 3, g_DrawObjPic.stObjectConfig.aObjectColor[stTempObjectInfo.ObjectColor]);		
				m_ListCtrl_ObjectInfo.SetItemText(i, 4, stTempObjectInfo.aObjectContext);
			}

            break;
        default:
            break;
        }
	}
    else if (msg->message == WM_KEYUP)
    {
        switch (msg->wParam)
        {
        case VK_CONTROL:
            if (g_DrawObjPic.m_iDrawStyle == 1)
            {
                g_DrawObjPic.m_bModifyObjRect = false;
                //g_DrawObjPic.m_bObjRectDrawed = true;
            }
            break;
        case VK_SHIFT:
            if (g_DrawObjPic.m_iDrawStyle == 0)
            {
                g_DrawObjPic.m_bModifyObjLabel = false;
            }
            break;
        default:
            break;
        }
    }

    m_ToolTipCtrl.RelayEvent(msg);
	return CDialog::PreTranslateMessage(msg);
}

void CObjectAnnoDlg::OnCbnSelchangeComboObjectType()
{
	// TODO: 在此添加控件通知处理程序代码
    //int iSel = ((CComboBox*)GetDlgItem(IDC_COMBO_ObjectType))->GetCurSel();
}

void CObjectAnnoDlg::OnBnClickedBtnNextFile()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.LoadNextFile();
}

void CObjectAnnoDlg::OnBnClickedBtnPlayForward()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.m_bPlayForward = true;
}

void CObjectAnnoDlg::OnBnClickedBtnPlayBackward()
{
	// TODO: 在此添加控件通知处理程序代码
	g_DrawObjPic.m_bPlayBackward = true;
}

void CObjectAnnoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
    UPDATE_EASYSIZE;
}

void CObjectAnnoDlg::ReadConfigParams(void)
{
    memset(&g_DrawObjPic.stObjectConfig, 0, sizeof(ObjectConfig));

    std::string strConfigFilePath = std::string(g_DrawObjPic.m_aExePath) + std::string("ObjectAnnCfg.xml");

	//read xml file
	CvFileStorage *fs = cvOpenFileStorage(strConfigFilePath.c_str(), 0, CV_STORAGE_READ);
	if (!fs)
	{
		AfxMessageBox("xml file read failed~!");
		return;
	}

	CvSeqReader reader;
	CvFileNode *root = cvGetRootFileNode(fs, 0);
	CvFileNode *node = cvGetFileNodeByName(fs, root, "ObjectType");

	cvStartReadSeq(node->data.seq, &reader);

	int iObjNum = cvReadIntByName(fs, node, "ObjectNum");
	for (int k = 0; k < iObjNum; k++)
	{
		char szObject[20];
		sprintf_s(szObject, "Object_%d", k + 1);

		strcpy(g_DrawObjPic.stObjectConfig.aObjectType[k], cvReadStringByName(fs, node, szObject));
	}

    g_DrawObjPic.stObjectConfig.iObjectNum = iObjNum;

	node = cvGetFileNodeByName(fs, root, "ObjectColor");

	cvStartReadSeq(node->data.seq, &reader);

	int iObjColorNum = cvReadIntByName(fs, node, "ObjectColorNum");
	for (int k = 0; k < iObjColorNum; k++)
	{
		char szObject[20];
		sprintf_s(szObject, "ObjectColor_%d", k + 1);

		strcpy(g_DrawObjPic.stObjectConfig.aObjectColor[k], cvReadStringByName(fs, node, szObject));
	}

	g_DrawObjPic.stObjectConfig.iObjectColorNum = iObjColorNum;

	node = cvGetFileNodeByName(fs, root, "ObjectPartType");

	cvStartReadSeq(node->data.seq, &reader);

	int iObjPartNum = cvReadIntByName(fs, node, "ObjectPartNum");
	for (int k = 0; k < iObjPartNum; k++)
	{
		char szObject[20];
		sprintf_s(szObject, "ObjectPart_%d", k + 1);

		strcpy(g_DrawObjPic.stObjectConfig.aObjectPartType[k], cvReadStringByName(fs, node, szObject));
	}

	g_DrawObjPic.stObjectConfig.iObjectPartNum = iObjPartNum;

	cvReleaseFileStorage(&fs);
}
void CObjectAnnoDlg::OnBnClickedRadioLabelBgarea()
{
    // TODO: Add your control notification handler code here
    g_DrawObjPic.m_bLabelBGArea = true;
    g_DrawObjPic.m_bLabelOcclution = false;
}

void CObjectAnnoDlg::OnBnClickedRadioLabelOcclutionarea()
{
    // TODO: Add your control notification handler code here
    g_DrawObjPic.m_bLabelBGArea = false;
    g_DrawObjPic.m_bLabelOcclution = true;
}

void CObjectAnnoDlg::OnCbnSelchangeComboModeselect()
{
    // TODO: Add your control notification handler code here
    int iSel = ((CComboBox*)GetDlgItem(IDC_COMBO_ModeSelect))->GetCurSel();

    g_DrawObjPic.m_bCheckMode = (bool)iSel;

    if (g_DrawObjPic.m_bCheckMode)
    {
        GetDlgItem(IDC_Btn_ImportVideo)->EnableWindow(FALSE);
        //GetDlgItem(IDC_COMBO_DrawStyle)->EnableWindow(FALSE);
    }
    else
    {
        //GetDlgItem(IDC_Btn_ImportVideo)->EnableWindow(TRUE);
        //GetDlgItem(IDC_COMBO_DrawStyle)->EnableWindow(TRUE);
    }

    //GetDlgItem(IDC_Btn_ReAnno)->EnableWindow(FALSE);
    //GetDlgItem(IDC_Btn_Undo)->EnableWindow(FALSE);
    //GetDlgItem(IDC_Btn_SaveObjInfo)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_PrevFile)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_NextFile)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_Play)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_SelectPlayROI)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_OriVideo)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_PlayAccelerate)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_PlayDecelerate)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_PlayForward)->EnableWindow(FALSE);
    GetDlgItem(IDC_Btn_PlayBackward)->EnableWindow(FALSE);
}

void CObjectAnnoDlg::OnNMDblclkListObjectinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    if (-1 != pNMItemActivate->iItem)
    {
        if (g_DrawObjPic.m_ImageShow != NULL)
        {
			CObjInfoDlg ObjectInfoDlg;

			ObjectInfoDlg.m_pObjAnnoWnd = this;
			ObjectInfoDlg.m_iObjectSel = pNMItemActivate->iItem;	
            
			ObjectInfoDlg.DoModal();
        }
    }

    *pResult = 0;
}

void CObjectAnnoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    CString strBuf;
    if (g_DrawObjPic.FileType == 1)
    {
        g_DrawObjPic.m_nFileIndex = m_SliderCtrl_FileProgress.GetPos() - 1;
        
        strBuf.Format("当前图片进度: %d / %d", g_DrawObjPic.m_nFileIndex + 1, g_DrawObjPic.m_vFileName.size());
    }
    else if (g_DrawObjPic.FileType == 2)
    {
        g_DrawObjPic.m_nVideoFrameIdx = m_SliderCtrl_FileProgress.GetPos() - 1;

        strBuf.Format("当前视频帧进度: %d / %d", g_DrawObjPic.m_nVideoFrameIdx + 1, g_DrawObjPic.m_nVideoFrameNum);

        g_DrawObjPic.m_bSliderChanged = true;
    }
    
    m_Static_FileProcess.SetWindowText(strBuf);

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CObjectAnnoDlg::DrawPic2HDC(IplImage *img, UINT ID)
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
void CObjectAnnoDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
    CDialog::OnSizing(fwSide, pRect);

    // TODO: Add your message handler code here
    EASYSIZE_MINSIZE(800, 700, fwSide, pRect);
}

void CObjectAnnoDlg::OnCbnSelchangeComboDrawstyle()
{
    // TODO: Add your control notification handler code here
    int iSel = ((CComboBox*)GetDlgItem(IDC_COMBO_DrawStyle))->GetCurSel();

	if (iSel == 0)
	{
		GetDlgItem(IDC_RADIO_LABEL_BGArea)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_LABEL_OcclutionArea)->EnableWindow(TRUE);
	}
	else if (iSel == 1)
	{
		GetDlgItem(IDC_RADIO_LABEL_BGArea)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_LABEL_OcclutionArea)->EnableWindow(FALSE);
	}

    g_DrawObjPic.m_iDrawStyle = iSel;
	g_DrawObjPic.ClearVariableState();
}

void CObjectAnnoDlg::OnNMClickListObjectinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
	//if (g_DrawObjPic.m_iObjectSel != pNMItemActivate->iItem)
	{
		g_DrawObjPic.m_iObjectSel = pNMItemActivate->iItem;
	}
	//else
	{
		if (pNMItemActivate->iItem != -1)
		{
			ObjectInfo stTempObjectInfo = g_DrawObjPic.m_vObjInfo[pNMItemActivate->iItem];

			m_ComboCtrl_ObjectType.SetCurSel(stTempObjectInfo.iObjectIndex);
			m_ComboCtrl_ObjectPartType.SetCurSel(stTempObjectInfo.iObjectPartIndex);
			m_ComboCtrl_ObjectColor.SetCurSel(stTempObjectInfo.ObjectColor);
			m_Edit_ObjectContext.SetWindowText(stTempObjectInfo.aObjectContext);

			g_DrawObjPic.m_ObjPos = cvPoint(0, 0);
			g_DrawObjPic.m_ObjCurPos = cvPoint(0, 0);
			g_DrawObjPic.ClearVariableState();
			g_DrawObjPic.m_bObjRectDrawed = true;
			memcpy(g_DrawObjPic.m_iObjectLabelMap, stTempObjectInfo.aObjectAreaLabelMap, OBJECT_BLOCK_Y_NUM * OBJECT_BLOCK_Y_NUM * sizeof(int));
		}
		else
		{
			g_DrawObjPic.ClearVariableState();
		}
	}

    *pResult = 0;
}

void CObjectAnnoDlg::OnCbnSelchangeComboObjectparttype()
{
	// TODO: Add your control notification handler code here
	int iSel = m_ComboCtrl_ObjectPartType.GetCurSel();

	if (iSel == g_DrawObjPic.stObjectConfig.iObjectPartNum)
	{
		iSel = -1;
		m_ComboCtrl_ObjectPartType.SetCurSel(iSel);
	}

	if (iSel == 2)
	{
		m_Edit_ObjectContext.EnableWindow(TRUE);
	}
	else
	{
		m_Edit_ObjectContext.EnableWindow(FALSE);
	}
}

void CObjectAnnoDlg::OnCbnSelchangeComboObjectcolor()
{
	// TODO: Add your control notification handler code here
	int iSel = m_ComboCtrl_ObjectColor.GetCurSel();

	if (iSel == g_DrawObjPic.stObjectConfig.iObjectColorNum)
	{
		iSel = -1;
		m_ComboCtrl_ObjectColor.SetCurSel(iSel);
	}
}
