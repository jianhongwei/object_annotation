#pragma once
#include "stdafx.h"
#include <vector>
#include <direct.h>
#include "opencv2/opencv.hpp"
#include "ObjAnnoDlg.h"

class DrawObjPic
{
public:
	DrawObjPic(void);
	~DrawObjPic(void);
public:
	bool                    m_bOnLButtonUp; //�ж��������ǵ���
    bool                    m_bOnLabelLBtnDown;
	HANDLE                  m_hObjThread;
	HANDLE                  m_hCaptureThread;

	CRect                   m_ImageRect;   //ͼ����ʾ����
	bool                    m_bExitThread; 
    bool                    m_bRunNextFile;

	CvPoint                 m_ObjPos;
    CvPoint                 m_ObjLTPt;
	CvPoint                 m_ObjCurPos;
    CvPoint                 m_ObjRBPt;

    int                     m_iStepX;
    int                     m_iStepY;
    int                     m_iObjectLabelMap[OBJECT_BLOCK_X_NUM * OBJECT_BLOCK_Y_NUM];//0��ʾ��������1��ʾ�ڵ�����

    char                    m_aExePath[_MAX_PATH]; //��ǰ����Ŀ¼
	char                    m_aFilePath[_MAX_PATH]; //��ǰ�ļ���Ŀ¼

	float                   m_fWidthScale;   //�ߴ�ͼƬ�Ŀ����
	float                   m_fHeightScale;  //�ߴ�ͼƬ�ĸ߱���
    float                   m_fRectWidthScale;
    float                   m_fRectHeightScale;

	int                     m_OriImageWidth;        //ԭ��ͼ��Ŀ�
	int                     m_OriImageHeight;       //ԭ��ͼ��ĸ�

	char*                   m_ImageShow;

	int                     m_OffSetToWindowRectX;  //��̬ͼƬ������ڴ��ڵ�λ��
	int                     m_OffSetToWindowRectY;

	CRITICAL_SECTION        m_PlayCriticalSection;

	CvPoint                 m_CurCursorPos;  //��ǰ��������λ��

	int                     m_nFileCnt;      //��ǰĿ¼��ͼƬ������ 
	std::vector<CString>    m_vFileName;          //��ǰĿ¼��ͼƬ�б�
	int                     m_nFileIndex;        //��ǰ���ʵڼ���ͼƬ
    char                    m_strCurFilePath[_MAX_PATH];

	std::vector<ObjectInfo> m_vObjInfo;//��ǰͼ���ϱ�ע��Ŀ����Ϣ
	int                     m_nObjAnnoNum;             //��ǰͼ���ϱ�ע��Ŀ����Ŀ

	HWND                    m_hWnd;       //��������
	HWND                    m_hZoomWnd;        //���ŷŴ�����ľ��
    CWnd                    *m_pObjAnnoWnd;

	UINT                    FileType;   //�����򿪵��ļ�����
	bool                    m_bPause;    // �Ƿ���ͣ
	bool                    m_bPlayRect;  //�Ƿ񲥷�����
    bool                    m_bPlayForward;
    bool                    m_bPlayBackward;
	bool                    m_bPlayRectWnd; //��������Ĵ����Ѿ���

	CvPoint                m_PlayPointTL;    //������������ ����ڿؼ� 
	CvPoint                m_PlayPointBR;
    CvRect                 m_PlayRect;      //�����ͼ��

	int                    m_nVideoFrameNum;//ȡ������Ƶ��֡��
	int                    m_nVideoFrameIdx;//��ǰ��Ƶ֡
	int                    m_nCtrlPlaySpeed;  // ����֡��
    int                    m_bSliderChanged;

    ObjectConfig           stObjectConfig;
    ObjectInfo             stObjectInfo;

    IplImage               *m_pSrcImage;
    IplImage               *m_pImageZoomShow;
	//IplImage			   *m_pObjZoomShow;
    
    bool                   m_bObjRectDrawed;
    bool                   m_bModifyObjRect;  //�Ƿ��޸�Ŀ���
    bool                   m_bModifyObjLabel;
    bool                   m_bObjInfoConfirmed;   //ȷ��
    bool                   m_bObjPtSelect[4]; //Ŀ����ĸ��� ˳ʱ�뷽��
    CvPoint                m_ObjModifyPt[4];
    bool                   m_bLabelBGArea;
    bool                   m_bLabelOcclution;

    bool                   m_bCheckMode; //�Ƿ�Ϊ�˲�ģʽ
    int                    m_iDrawStyle; //��ͼ��ʽ  0:դ��  1:ƽ���ı���
    int                    m_iObjectSel;

    int                    m_iObjectAngleX;
    int                    m_iObjectAngleY;

    bool                   m_bObjectInfoSaved;

	CvRect				   m_stShowROI;

	//ObjectInfo             m_stObjectInfoSel;

public:
	bool UpdateImageRect(); //��ʼ��
	bool LoadImageScource(UINT flags);  //����ͼƬ

	static DWORD WINAPI ObjThreadProc(LPVOID pParam);
	DWORD ObjThreadProcFunc();
	static DWORD WINAPI  EngineThread(LPVOID pParam);// ��Ƶ�ɼ��߳�
	DWORD ThreadFunc();

	void DrawOnLButtonDown(UINT nFlags, CPoint point);
	void DrawOnLButtonUp(UINT nFlags, CPoint point);
	void DrawOnMouseMove(UINT nFlags, CPoint point);

	void DrawObjectRect(); 

	void ReDrawPic();  //���±�ע
	void UndoDrawObj(); //������ǰ����

	void WriteObjectAnnoInfo();
    void ReadObjectAnnoInfo();
	void LoadNextFile();  //��һ�ε���ͼƬ
	void LoadPrevFile();  //��һ�ε���
	void RealPlay();
	void StopPlay();
	void RealPlayRect();
	void ReRealPlay(); //��ԭ����
	void PlayAccelerate(); //����
	void PlayDecelerate(); //����
	void ExitThread();  //�˳�
	void ShowRectZoomImage();  //��һƬС����Ŵ�
    void ReadImageFile(char *pImgPath);
    void ReadVideoFile();
    void ShowObjectModel(int iAnlgeX, int iAngleY); 
    void ReCreateThread(void);
    bool IsRectIntersection(CRect rt1, CRect rt2);
    void ClearVariableState(void);
    void ClearObjectInfo(void);
	void DrawObjectRectOrGrid(IplImage *pShowImage);
};