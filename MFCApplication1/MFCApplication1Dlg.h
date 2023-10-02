
// MFCApplication1Dlg.h: 헤더 파일
//

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/geometry.h>
#include <iostream>
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
using namespace cv;

using namespace dlib;
using namespace std;
// CMFCApplication1Dlg 대화 상자
class CMFCApplication1Dlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic pic;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	HBITMAP Mat2DIB(Mat* Image);
	int detect(array2d<bgr_pixel> *img);
	VideoCapture* capture;
	Mat mat_frame;
	CImage cimage_mfc;
	frontal_face_detector detector;
	shape_predictor sp;
	full_object_detection shape;
	static UINT drawthr(LPVOID x);
	static UINT iothr(LPVOID x);
	static UINT iothr2(LPVOID x);
	static UINT processthr(LPVOID x);
	static UINT beepthr(LPVOID x);
	void presskey(char ch, int lang);
	double eye_x, eye_y,eye_r;
	bool eye_updated = false;
	bool eye_updated2 = false;
	int beepf = 0;
	int cur_x, cur_y;
	int win_x, win_y;
	int currentkey=-1,currentlang=-1;
	double eye_x_min, eye_x_max, eye_y_min, eye_y_max;
	
	int reset = -2;//0:done
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClickedReset();
};
