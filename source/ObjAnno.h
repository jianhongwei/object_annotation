// CarCaptruePicture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CCarCaptruePictureApp:
// �йش����ʵ�֣������ CarCaptruePicture.cpp
//

class CObjectAnnoApp : public CWinApp
{
public:
	CObjectAnnoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CObjectAnnoApp theApp;
