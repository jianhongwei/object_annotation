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
	bool                    m_bOnLButtonUp; //判断鼠标左键是弹起
    bool                    m_bOnLabelLBtnDown;
	HANDLE                  m_hObjThread;
	HANDLE                  m_hCaptureThread;

	CRect                   m_ImageRect;   //图像显示区域
	bool                    m_bExitThread; 
    bool                    m_bRunNextFile;

	CvPoint                 m_ObjPos;
    CvPoint                 m_ObjLTPt;
	CvPoint                 m_ObjCurPos;
    CvPoint                 m_ObjRBPt;

    int                     m_iStepX;
    int                     m_iStepY;
    int                     m_iObjectLabelMap[OBJECT_BLOCK_X_NUM * OBJECT_BLOCK_Y_NUM];//0表示背景索引1表示遮挡索引

    char                    m_aExePath[_MAX_PATH]; //当前程序目录
	char                    m_aFilePath[_MAX_PATH]; //当前文件的目录

	float                   m_fWidthScale;   //尺寸图片的宽比例
	float                   m_fHeightScale;  //尺寸图片的高比例
    float                   m_fRectWidthScale;
    float                   m_fRectHeightScale;

	int                     m_OriImageWidth;        //原来图像的宽
	int                     m_OriImageHeight;       //原来图像的高

	char*                   m_ImageShow;

	int                     m_OffSetToWindowRectX;  //静态图片框相对于窗口的位置
	int                     m_OffSetToWindowRectY;

	CRITICAL_SECTION        m_PlayCriticalSection;

	CvPoint                 m_CurCursorPos;  //当前鼠标的坐标位置

	int                     m_nFileCnt;      //当前目录下图片的数量 
	std::vector<CString>    m_vFileName;          //当前目录下图片列表
	int                     m_nFileIndex;        //当前访问第几张图片
    char                    m_strCurFilePath[_MAX_PATH];

	std::vector<ObjectInfo> m_vObjInfo;//当前图像上标注的目标信息
	int                     m_nObjAnnoNum;             //当前图像上标注的目标数目

	HWND                    m_hWnd;       //父窗体句柄
	HWND                    m_hZoomWnd;        //播放放大区域的句柄
    CWnd                    *m_pObjAnnoWnd;

	UINT                    FileType;   //表明打开的文件类型
	bool                    m_bPause;    // 是否暂停
	bool                    m_bPlayRect;  //是否播放区域
    bool                    m_bPlayForward;
    bool                    m_bPlayBackward;
	bool                    m_bPlayRectWnd; //播放区域的窗口已经定

	CvPoint                m_PlayPointTL;    //播放区域坐标 相对于控件 
	CvPoint                m_PlayPointBR;
    CvRect                 m_PlayRect;      //相对于图像

	int                    m_nVideoFrameNum;//取到的视频总帧数
	int                    m_nVideoFrameIdx;//当前视频帧
	int                    m_nCtrlPlaySpeed;  // 控制帧速
    int                    m_bSliderChanged;

    ObjectConfig           stObjectConfig;
    ObjectInfo             stObjectInfo;

    IplImage               *m_pSrcImage;
    IplImage               *m_pImageZoomShow;
	//IplImage			   *m_pObjZoomShow;
    
    bool                   m_bObjRectDrawed;
    bool                   m_bModifyObjRect;  //是否修改目标框
    bool                   m_bModifyObjLabel;
    bool                   m_bObjInfoConfirmed;   //确定
    bool                   m_bObjPtSelect[4]; //目标框四个点 顺时针方向
    CvPoint                m_ObjModifyPt[4];
    bool                   m_bLabelBGArea;
    bool                   m_bLabelOcclution;

    bool                   m_bCheckMode; //是否为核查模式
    int                    m_iDrawStyle; //画图样式  0:栅格  1:平行四边形
    int                    m_iObjectSel;

    int                    m_iObjectAngleX;
    int                    m_iObjectAngleY;

    bool                   m_bObjectInfoSaved;

	CvRect				   m_stShowROI;

	//ObjectInfo             m_stObjectInfoSel;

public:
	bool UpdateImageRect(); //初始化
	bool LoadImageScource(UINT flags);  //导入图片

	static DWORD WINAPI ObjThreadProc(LPVOID pParam);
	DWORD ObjThreadProcFunc();
	static DWORD WINAPI  EngineThread(LPVOID pParam);// 视频采集线程
	DWORD ThreadFunc();

	void DrawOnLButtonDown(UINT nFlags, CPoint point);
	void DrawOnLButtonUp(UINT nFlags, CPoint point);
	void DrawOnMouseMove(UINT nFlags, CPoint point);

	void DrawObjectRect(); 

	void ReDrawPic();  //重新标注
	void UndoDrawObj(); //撤销当前操作

	void WriteObjectAnnoInfo();
    void ReadObjectAnnoInfo();
	void LoadNextFile();  //下一次导入图片
	void LoadPrevFile();  //上一次导入
	void RealPlay();
	void StopPlay();
	void RealPlayRect();
	void ReRealPlay(); //还原播放
	void PlayAccelerate(); //加速
	void PlayDecelerate(); //减速
	void ExitThread();  //退出
	void ShowRectZoomImage();  //将一片小区域放大
    void ReadImageFile(char *pImgPath);
    void ReadVideoFile();
    void ShowObjectModel(int iAnlgeX, int iAngleY); 
    void ReCreateThread(void);
    bool IsRectIntersection(CRect rt1, CRect rt2);
    void ClearVariableState(void);
    void ClearObjectInfo(void);
	void DrawObjectRectOrGrid(IplImage *pShowImage);
};