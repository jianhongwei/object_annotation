#pragma once
#include "ObjAnnoDlg.h"
#include "afxwin.h"

// CObjInfoDlg dialog

class CObjInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CObjInfoDlg)

public:
	CObjInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CObjInfoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ObjInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonPrevobj();
	afx_msg void OnBnClickedButtonNextobj();
	afx_msg void OnCbnSelchangeComboObjecttypesel();
public:
	CWnd        *m_pObjAnnoWnd;
	IplImage    *m_pObjInfoImage;
	CComboBox   m_ComboCtrl_ObjectTypeSel;
	CComboBox   m_ComboCtrl_ObjectColorSel;
	CComboBox   m_ComboCtrl_ObjectPartTypeSel;
	int			m_iObjectSel;
	int         m_iObjectTypeSel;
	int			m_iObjectPartSel;
	int			m_iObjectColorSel;
	int			m_iObjectAngleX;
	int			m_iObjectAngleY;
	CEdit		m_Edit_LPContext;
public:
	void ShowObjectInfo();
	void DrawPic2HDC(IplImage *img, UINT ID);
	afx_msg void OnCbnSelchangeComboObjectcolorsel();
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeComboObjectparttypesel();
};
