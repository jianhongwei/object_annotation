PROJECT(ObjAnno)

SET(SRCS_ObjAnno
	EasySize.h
	CvvImage.h
	CvvImage.cpp
	DrawObjPic.h
	DrawObjPic.cpp
	ObjAnno.h
	ObjAnno.cpp
	ObjAnnoDlg.h
	ObjAnnoDlg.cpp
	ObjInfoDlg.h
	ObjInfoDlg.cpp
	stdafx.h
	stdafx.cpp
	resource.h
	ObjAnno.rc
)

SET(CMAKE_MFC_FLAG 2)
ADD_EXECUTABLE(ObjAnno WIN32 ${SRCS_ObjAnno})
TARGET_LINK_LIBRARIES(ObjAnno ${OpenCV_LIBS}) 
