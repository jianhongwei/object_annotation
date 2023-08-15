// CObjectAnnoDlg.h : 头文件
//
#pragma once
#include "afxwin.h"
#include <vector>
#include "resource.h"
#include "DrawObjPic.h"
#include "afxcmn.h"

// CObjectAnnoDlg 对话框
class CObjectAnnoDlg : public CDialog
{
    DECLARE_EASYSIZE;
// 构造
public:
	CObjectAnnoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_OBJECTANNO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnImportImage();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnReAnno();
	afx_msg void OnBnClickedBtnPreFile();
	afx_msg void OnBnClickedBtnUndo();
	afx_msg void OnBnClickedBtnSaveObjInfo();
	afx_msg void OnBnClickedBtnImportVideo();
	afx_msg LRESULT OnCWndDealMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnPlayAccelerate();
	afx_msg void OnBnClickedBtnSelectPlayROI();
	afx_msg void OnBnClickedBtnOriVideo();
	afx_msg void OnBnClickedBtnPlayDecelerate();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG *msg);
	afx_msg void OnCbnSelchangeComboObjectType();
	afx_msg void OnBnClickedBtnNextFile();
	afx_msg void OnBnClickedBtnPlayForward();
	afx_msg void OnBnClickedBtnPlayBackward();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedRadioLabelBgarea();
    afx_msg void OnBnClickedRadioLabelOcclutionarea();
    afx_msg void OnCbnSelchangeComboModeselect();
    afx_msg void OnCbnSelchangeComboDrawstyle();
    afx_msg void OnNMDblclkListObjectinfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnNMClickListObjectinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboObjectparttype();
	afx_msg void OnCbnSelchangeComboObjectcolor();
public:
    void ReadConfigParams(void);
    void DrawPic2HDC(IplImage *img, UINT ID);
    CListCtrl m_ListCtrl_ObjectInfo;
    CComboBox m_ComboCtrl_ObjectType;
    CToolTipCtrl m_ToolTipCtrl;
    CSliderCtrl m_SliderCtrl_FileProgress;
    CStatic m_Static_FileProcess;
    CStatic m_Static_ImageShow;
    CComboBox m_ComboCtrl_DrawStyle;
    CComboBox m_ComboCtrl_ObjectColor;
    CEdit m_Edit_ObjectContext;
	BOOL  m_bShowObjInfoDlg;
	CComboBox m_ComboCtrl_ObjectPartType;
	CButton m_RadioCtrl_LabelBGArea;
	CComboBox m_ComboBox_VehicleLogo_Type;
};
