// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0501		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0501 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0501	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#define WM_MSG_CAR                      WM_USER+101

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
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
	int						iObjectIndex;          //目标类别索引
	enObjectColor			ObjectColor; 
	char					aObjectType[_MAX_PATH];          //目标类别

	int						iObjectAngleY;
	int						iObjectAngleX;

	CvPoint					stObjectRectPt[4]; //目标框位置

	int						aObjectAreaLabelMap[OBJECT_BLOCK_X_NUM * OBJECT_BLOCK_Y_NUM];

	BOOL					bHaveContext;
	char					aObjectContext[_MAX_PATH];

	int						iDrawType;

	BOOL					bObjectPart;
	int						iObjectPartIndex;
	char					aObjectPartType[_MAX_PATH];
}ObjectInfo;

//目标标注配置信息
typedef struct tagObjectConfig
{
	int		iObjectNum;
	char	aObjectType[OBJECT_NUM_MAX][_MAX_PATH];

	int     iObjectPartNum;
	char	aObjectPartType[OBJECT_PART_NUM_MAX][_MAX_PATH];

	int		iObjectColorNum;
	char	aObjectColor[OBJECT_COLOR_NUM_MAX][_MAX_PATH];
}ObjectConfig;

