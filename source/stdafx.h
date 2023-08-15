// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// ��Ŀ�ض��İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ��ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0501		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0501		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0501 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0501	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

#define WM_MSG_CAR                      WM_USER+101

// �ر� MFC ��ĳЩ��������������ȫ���Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ���ĺͱ�׼���
#include <afxext.h>         // MFC ��չ
#include <afxdisp.h>        // MFC �Զ�����

#include <afxdtctl.h>		// Internet Explorer 4 �����ؼ��� MFC ֧��
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows �����ؼ��� MFC ֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "CvvImage.h"
#include "EasySize.h"

#define OBJECT_NUM_MAX          50
#define OBJECT_PART_NUM_MAX     10
#define OBJECT_COLOR_NUM_MAX    20
#define Image_Zoom_Width        300
#define Image_Zoom_Height       240
#define OBJECT_BLOCK_X_NUM      10
#define OBJECT_BLOCK_Y_NUM      10

const CvScalar RED       = cvScalar(0, 0, 255);
const CvScalar WHITE     = cvScalar(255, 255, 255);
const CvScalar YELLOW    = cvScalar(0, 255, 255);
const CvScalar CYAN      = cvScalar(255, 255, 0);
const CvScalar MAGENTA   = cvScalar(255, 0, 255);
const CvScalar ORANGE    = cvScalar(0, 165, 255);
const CvScalar BLACK     = cvScalar(0, 0, 0);
const CvScalar PINK      = cvScalar(230, 130, 255);
const CvScalar BLUE      = cvScalar(255, 0, 0);
const CvScalar SKYBLUE   = cvScalar(235, 206, 135);
const CvScalar LIGHTBLUE = cvScalar(255, 255,160);
const CvScalar GREEN     = cvScalar(0, 255, 0);

typedef enum enObjectColor
{
	OBJ_Unkown = -1,
	OBJ_BLUE,
	OBJ_YELLOW,
	OBJ_BLACK,
	OBJ_WHITE,
	OBJ_RED,
	OBJ_GREEN,
	OBJ_GRAY,
	OBJ_SILVER,
	OBJ_WATHET,
	OBJ_CHAMPAGNE
};

typedef struct tagObjectInfo
{
	int						iObjectIndex;          //Ŀ���������
	enObjectColor			ObjectColor; 
	char					aObjectType[_MAX_PATH];          //Ŀ�����

	int						iObjectAngleY;
	int						iObjectAngleX;

	CvPoint					stObjectRectPt[4]; //Ŀ���λ��

	int						aObjectAreaLabelMap[OBJECT_BLOCK_X_NUM * OBJECT_BLOCK_Y_NUM];

	BOOL					bHaveContext;
	char					aObjectContext[_MAX_PATH];

	int						iDrawType;

	BOOL					bObjectPart;
	int						iObjectPartIndex;
	char					aObjectPartType[_MAX_PATH];
}ObjectInfo;

//Ŀ���ע������Ϣ
typedef struct tagObjectConfig
{
	int		iObjectNum;
	char	aObjectType[OBJECT_NUM_MAX][_MAX_PATH];

	int     iObjectPartNum;
	char	aObjectPartType[OBJECT_PART_NUM_MAX][_MAX_PATH];

	int		iObjectColorNum;
	char	aObjectColor[OBJECT_COLOR_NUM_MAX][_MAX_PATH];
}ObjectConfig;

