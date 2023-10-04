
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#pragma comment(lib,"opencv_world460.lib")
#pragma comment(lib,"opencv_world460d.lib")
#pragma comment(lib,"dlib19.24.2_release_64bit_msvc1932.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class Serial
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;

public:
	//Initialize Serial communication with the given COM port
	Serial(const wchar_t* portName);
	//Close the connection
	~Serial();
	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	int ReadData(char* buffer, unsigned int nbChar);
	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(const char* buffer, unsigned int nbChar);
	//Check if we are actually connected
	bool IsConnected();


};
Serial::Serial(const wchar_t* portName)
{
	//We're not yet connected
	this->connected = false;

	//Try to connect to the given port throuh CreateFile
	this->hSerial = CreateFileW(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {

			//Print Error if neccessary
			printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			printf("failed to get current serial parameters!");
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_115200;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			//Setting the DTR to Control_Enable ensures that the Arduino is properly
			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				//If everything went fine we're connected
				this->connected = true;
				//Flush any remaining characters in the buffers 
				PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				//We wait 2s as the arduino board will be reseting
				Sleep(2000);
			}
		}
	}

}

Serial::~Serial()
{
	//Check if we are connected before trying to disconnect
	if (this->connected)
	{
		//We're no longer connected
		this->connected = false;
		//Close the serial handler
		CloseHandle(this->hSerial);
	}
}

int Serial::ReadData(char* buffer, unsigned int nbChar)
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &this->errors, &this->status);

	//Check if there is something to read
	if (this->status.cbInQue > 0)
	{
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (this->status.cbInQue > nbChar)
		{
			toRead = nbChar;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL))
		{
			return bytesRead;
		}

	}

	//If nothing has been read, or that an error was detected return 0
	return 0;

}


bool Serial::WriteData(const char* buffer, unsigned int nbChar)
{
	DWORD bytesSend;

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void*)buffer, nbChar, &bytesSend, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
		return true;
}

bool Serial::IsConnected()
{
	//Simply return the connection status
	return this->connected;
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_pic, pic);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_reset, &CMFCApplication1Dlg::OnClickedReset)
//	ON_WM_KEYDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	capture = new VideoCapture(0);
	if (!capture->isOpened())
	{
		MessageBox(_T("웹캠을 열수 없습니다. \n"));
	}   
	capture->set(CAP_PROP_FRAME_WIDTH, 320);
	capture->set(CAP_PROP_FRAME_HEIGHT, 240);
	
	SetTimer(1000, 20, NULL);


	POINT pt;
	::GetCursorPos(&pt);
	cur_x = pt.x; cur_y = pt.y;
	win_x = GetSystemMetrics(SM_CXSCREEN);
	win_y = GetSystemMetrics(SM_CYSCREEN);


	CWinThread* p1 = NULL,*p2=NULL,*p3=NULL,*p4=NULL,*p5=NULL;
	p1 = AfxBeginThread(drawthr, this);
	p2 = AfxBeginThread(iothr, this);
	p3 = AfxBeginThread(processthr, this);
	p4 = AfxBeginThread(beepthr, this);
	p5= AfxBeginThread(iothr2, this);
	if (p1 == NULL) AfxMessageBox(L"Error-drawthr");
	if (p2 == NULL) AfxMessageBox(L"Error-iothr");
	if (p3 == NULL) AfxMessageBox(L"Error-processthr");
	if (p4 == NULL) AfxMessageBox(L"Error-beepthr");
	if (p5 == NULL) AfxMessageBox(L"Error-iothr2");
	detector = get_frontal_face_detector();
	deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
	wchar_t buf[200];
	for (int i = 0; i < 10; i++) {
		wsprintf(buf, L"\\\\.\\COM%d", i + 1);
		serial = new Serial(buf);
		if (((Serial*)serial)->IsConnected()) {
			printf("%d serial\n", i); break;
		}
		else {
			delete serial;
			serial = 0;
		}
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - win_x + 1) / 2;
		int y = (rect.Height() - win_y + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication1Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

HBITMAP CMFCApplication1Dlg::Mat2DIB(Mat* Image)
{
	HBITMAP hbmp = NULL;
	int bpp = Image->channels() * 8;

	//Mat tmp;
	//cvtColor(*Image, tmp, CV_BGRA2BGR);

	BITMAPINFO bmpInfo = { 0 };
	LONG lBmpSize = Image->rows * Image->cols * 3;
	bmpInfo.bmiHeader.biBitCount = bpp;
	bmpInfo.bmiHeader.biHeight = Image->rows;
	bmpInfo.bmiHeader.biWidth = Image->cols;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// Pointer to access the pixels of bitmap
	BYTE* pPixels = 0, * pP;
	hbmp = CreateDIBSection(NULL, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS, (void**)&pPixels, NULL, 0);

	if (!hbmp)
		return hbmp; // return if invalid bitmaps

	   //SetBitmapBits( hBitmap, lBmpSize, pBits);
	   // Directly Write
	int left_width = ((bpp * Image->cols + 31) / 32) * 4;
	pP = pPixels;
	for (int y = Image->rows - 1, row = 0; row < Image->rows; row++, y--) {
		for (int x = 0, col = 0; col < Image->cols; x += 3, col++) {
			pP[x] = Image->at<Vec3b>(y, col).val[0];
			pP[x + 1] = Image->at<Vec3b>(y, col).val[1];
			pP[x + 2] = Image->at<Vec3b>(y, col).val[2];
		}
		pP += left_width;
	}
	return hbmp;
}


UINT CMFCApplication1Dlg::drawthr(LPVOID x) {

	// 윈도우 핸들을 CDialog 형식으로 캐스팅한다.
	CMFCApplication1Dlg* pDialog = (CMFCApplication1Dlg*)x;
	int f = pDialog->win_x;
	int g = pDialog->win_y;
	int fg = f * g;
	int fg4 = fg * 4;
	HDC hDC,hDC2;
	HBITMAP hBmp,hBmp2;
	int* ohBmp_raw = new int[fg];
	hDC = CreateCompatibleDC(0);
	hBmp = CreateCompatibleBitmap(::GetDC(0), f, g);
	SelectObject(hDC, hBmp);
	BitBlt(hDC, 0, 0, f, g, ::GetDC(0), 0, 0, SRCCOPY);
	GetBitmapBits(hBmp, fg4, ohBmp_raw);
	::ReleaseDC(0, hDC); DeleteObject(hBmp);
	int* hBmp_raw = new int[fg];
	int* temp = new int[fg];
	int* hBmpo_raw = new int[fg];
	memset(hBmpo_raw, 0, 4);
	HFONT oFont,sFont;
	oFont = CreateFontA(f / 40, 0, 0, 0, 500, false, false,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	sFont = CreateFontA(f / 80, 0, 0, 0, 500, false, false,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	HPEN rpen, cpen;
	rpen = CreatePen(PS_SOLID, 8, RGB(255, 0, 0));
	cpen = CreatePen(PS_SOLID, 4, RGB(0, 255, 255));
	while (1) {
		hDC = CreateCompatibleDC(0);
		hBmp = CreateCompatibleBitmap(::GetDC(0), f, g);
		SelectObject(hDC, hBmp);
		SetBkMode(hDC, TRANSPARENT);
		if (pDialog->reset < 1) {
			POINT pt;
			::GetCursorPos(&pt);
			pDialog->cur_x = pt.x; pDialog->cur_y = pt.y;
			SelectObject(hDC, rpen);
			MoveToEx(hDC, pDialog->cur_x - 10, pDialog->cur_y, NULL);
			LineTo(hDC, pDialog->cur_x + 10, pDialog->cur_y);
			MoveToEx(hDC, pDialog->cur_x, pDialog->cur_y - 10, NULL);
			LineTo(hDC, pDialog->cur_x, pDialog->cur_y + 10);
			SelectObject(hDC, cpen);
			MoveToEx(hDC, pDialog->cur_x - 10, pDialog->cur_y, NULL);
			LineTo(hDC, pDialog->cur_x + 10, pDialog->cur_y);
			MoveToEx(hDC, pDialog->cur_x, pDialog->cur_y - 10, NULL);
			LineTo(hDC, pDialog->cur_x, pDialog->cur_y + 10);

			if (pDialog->reset == -2) {
				SetTextColor(hDC, RGB(255, 0, 0));
				for (int j = -f / 1600; j <= f / 1600+2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600+2; k += f / 800) {
						SelectObject(hDC, oFont);
						TextOutA(hDC, 10+j, 10 + k, "초기화를 먼저 하세요", 21);
					}
				}
				SetTextColor(hDC, RGB(0, 255, 255));
				SelectObject(hDC, oFont);
				TextOutA(hDC, 10, 10, "초기화를 먼저 하세요", 21);
			}
		}
		else {
			if (pDialog->reset == 1) {
				
				SelectObject(hDC, oFont);
				SelectObject(hDC, rpen);
				SetTextColor(hDC, RGB(255, 0, 0));
				MoveToEx(hDC, f/2+10, 10, NULL);
				LineTo(hDC, f/2-10, 10);
				MoveToEx(hDC, f/2, 0, NULL);
				LineTo(hDC, f/2, 20);
				for (int j = -f / 1600; j <= f / 1600 + 2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600 + 2; k += f / 800) {
						TextOutA(hDC, 0.425 * f - 10 + j, 10+k, "위를 보세요", 12);
					}
				}
				SelectObject(hDC, cpen);
				SetTextColor(hDC, RGB(0, 255, 255));
				MoveToEx(hDC, f / 2 + 10, 10, NULL);
				LineTo(hDC, f / 2 - 10, 10);
				MoveToEx(hDC, f / 2, 0, NULL);
				LineTo(hDC, f / 2, 20);
				TextOutA(hDC, 0.425 * f - 10, 10, "위를 보세요", 12);
			}
			else if (pDialog->reset == 2) {
				SelectObject(hDC, rpen);
				SelectObject(hDC, oFont);
				SetTextColor(hDC, RGB(255, 0, 0));
				MoveToEx(hDC, f / 2 + 10, g - 10, NULL);
				LineTo(hDC, f / 2 - 10, g - 10);
				MoveToEx(hDC, f / 2, g, NULL);
				LineTo(hDC, f / 2, g - 20);
				for (int j = -f / 1600; j <= f / 1600 + 2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600 + 2; k += f / 800) {
						TextOutA(hDC, 0.425 * f - 10+j, g-f/40-10+k, "아래를 보세요", 14);
					}
				}
				SelectObject(hDC, cpen);
				SetTextColor(hDC, RGB(0, 255, 255));
				MoveToEx(hDC, f / 2 + 10, g - 10, NULL);
				LineTo(hDC, f / 2 - 10, g - 10);
				MoveToEx(hDC, f / 2, g, NULL);
				LineTo(hDC, f / 2, g - 20);
				TextOutA(hDC, 0.425*f-10, g - f / 40 - 10, "아래를 보세요", 14);
			}
			else if (pDialog->reset == 3) {
				
				SelectObject(hDC, oFont);
				SelectObject(hDC, rpen);
				SetTextColor(hDC, RGB(255, 0, 0));
				MoveToEx(hDC, 0, g/2, NULL);
				LineTo(hDC, 20, g/2);
				MoveToEx(hDC, 10, g/2-10, NULL);
				LineTo(hDC, 10, g/2+10);
				for (int j = -f / 1600; j <= f / 1600 + 2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600 + 2; k += f / 800) {
						TextOutA(hDC, 10+j, g/2 +k, "왼쪽을 보세요", 14);
					}
				}
				SelectObject(hDC, cpen);
				SetTextColor(hDC, RGB(0, 255, 255));
				MoveToEx(hDC, 0, g / 2, NULL);
				LineTo(hDC, 20, g / 2);
				MoveToEx(hDC, 10, g / 2 - 10, NULL);
				LineTo(hDC, 10, g / 2 + 10);
				TextOutA(hDC, 10, g/2, "왼쪽을 보세요", 14);
			}
			else if (pDialog->reset == 4) {
				
				SelectObject(hDC, oFont);
				SelectObject(hDC, rpen);
				SetTextColor(hDC, RGB(255, 0, 0));
				MoveToEx(hDC, f, g / 2, NULL);
				LineTo(hDC, f-20, g / 2);
				MoveToEx(hDC, f-10, g / 2 - 10, NULL);
				LineTo(hDC, f-10, g / 2 + 10);
				for (int j = -f / 1600; j <= f / 1600 + 2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600 + 2; k += f / 800) {
						TextOutA(hDC, 0.85 * f - 10 + j, g/2+ k, "오른쪽 보세요", 14);
					}
				}
				SelectObject(hDC, cpen);
				SetTextColor(hDC, RGB(0, 255, 255));
				MoveToEx(hDC, f, g / 2, NULL);
				LineTo(hDC, f - 20, g / 2);
				MoveToEx(hDC, f - 10, g / 2 - 10, NULL);
				LineTo(hDC, f - 10, g / 2 + 10);
				TextOutA(hDC, 0.85 * f - 10, g/2, "오른쪽 보세요", 14);
			}
			else if (pDialog->reset == 5) {

				SelectObject(hDC, oFont);
				SelectObject(hDC, rpen);
				SetTextColor(hDC, RGB(255, 0, 0));
				MoveToEx(hDC, f / 2, g / 2, NULL);
				LineTo(hDC, f / 2 - 20, g / 2);
				MoveToEx(hDC, f / 2 - 10, g / 2 - 10, NULL);
				LineTo(hDC, f / 2 - 10, g / 2 + 10);
				for (int j = -f / 1600; j <= f / 1600 + 2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600 + 2; k += f / 800) {
						TextOutA(hDC, 0.425 * f - 10 + j, g / 2 + k, "가운데 보세요", 14);
					}
				}
				SelectObject(hDC, cpen);
				SetTextColor(hDC, RGB(0, 255, 255));
				MoveToEx(hDC, f/2, g / 2, NULL);
				LineTo(hDC, f/2 - 20, g / 2);
				MoveToEx(hDC, f/2 - 10, g / 2 - 10, NULL);
				LineTo(hDC, f/2 - 10, g / 2 + 10);
				TextOutA(hDC, 0.425 * f - 10, g / 2, "가운데 보세요", 14);
			}
			else if (pDialog->reset == 6) {
				SelectObject(hDC, oFont);
				SetTextColor(hDC, RGB(255, 0, 0));
				for (int j = -f / 1600; j <= f / 1600 + 2; j += f / 800) {
					for (int k = -f / 1600; k <= f / 1600 + 2; k += f / 800) {
						TextOutA(hDC, 10+j, 10+k, "뭔가 잘못되었으니 다시 초기화하세요", 36);
					}
				}
				SetTextColor(hDC, RGB(0, 255, 255));

				TextOutA(hDC, 10, 10, "뭔가 잘못되었으니 다시 초기화하세요", 36);
			}
			
		}
		GetBitmapBits(hBmp, fg4, hBmp_raw);
		hDC2 = CreateCompatibleDC(0);
		hBmp2 = CreateCompatibleBitmap(::GetDC(0), f, g);
		SelectObject(hDC2, hBmp2);
		BitBlt(hDC2, 0, 0, f, g, ::GetDC(0), 0, 0, SRCCOPY);
		//
		swap(temp, ohBmp_raw);
		GetBitmapBits(hBmp2, fg4, ohBmp_raw);
		for (int i = 0; i < fg; i++) if (hBmpo_raw[i] != 0) ohBmp_raw[i] = temp[i];
		memcpy(temp, hBmp_raw, fg4);
		for (int i = 0; i < fg; i++) if (temp[i] == 0) temp[i]=ohBmp_raw[i];
		SetBitmapBits(hBmp2, fg4, temp);
		BitBlt(::GetDC(0), 0, 0, f, g, hDC2, 0, 0, SRCCOPY);
		swap(hBmpo_raw, hBmp_raw);
		DeleteObject(hBmp); DeleteObject(hBmp2);
		::ReleaseDC(0, hDC2); ::ReleaseDC(0, hDC);
		Sleep(50);
	}
	
	return 0;
}

void Hangul_On(HWND hWnd, bool bSetHan){
	HIMC hIMC;
	DWORD dwConv, dwSent, dwTemp;

	hIMC = ImmGetContext(hWnd);
	ImmGetConversionStatus(hIMC, &dwConv, &dwSent);

	int flag = dwConv & IME_CMODE_NATIVE;
	if (bSetHan) {
		if (flag)   return;
		dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
		dwTemp |= IME_CMODE_NATIVE;
	}
	else {
		if (!flag) return;
		dwTemp = dwConv & ~IME_CMODE_LANGUAGE;
	}
	dwConv = dwTemp;
	ImmSetConversionStatus(hIMC, dwConv, dwSent);
	ImmReleaseContext(hWnd, hIMC);
}

void presskey(DWORD type,DWORD dwFlags,WORD wvk=0) {
	INPUT in;
	if (type == INPUT_MOUSE) {
		in.type = type;
		in.mi.dx = 0;
		in.mi.dy = 0;
		in.mi.mouseData = 0;
		in.mi.time = 0;
		in.mi.dwExtraInfo = 0;
		in.mi.dwFlags = dwFlags; SendInput(1, &in, sizeof in);
	}
	else if (type == INPUT_KEYBOARD) {
		in.type = INPUT_KEYBOARD;
		in.ki.wVk = wvk;
		in.ki.wScan = 0;
		in.ki.dwFlags = dwFlags;
		in.ki.time = 0;
		in.ki.dwExtraInfo = 0; SendInput(1, &in, sizeof in);
	}
}

UINT CMFCApplication1Dlg::iothr2(LPVOID x) {
	CMFCApplication1Dlg* pDialog = (CMFCApplication1Dlg*)x;
	bool pressed = false;
	bool shifted = false;
	while (1) {
		if (pDialog->currentkey != -1) {
			int key = pDialog->currentkey; pDialog->currentkey = -1;
 			int lang = pDialog->currentlang;
			pDialog->beepf = 880;
			if (key == 0) {
				if (!pressed) ::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTDOWN);
				pressed = false;
				Sleep(50);
				::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTUP);
			}
			else if (key == 1) {
				::presskey(INPUT_MOUSE, MOUSEEVENTF_RIGHTDOWN);
				Sleep(50);
				::presskey(INPUT_MOUSE, MOUSEEVENTF_RIGHTUP);
			}
			else if (key == 2) {
				::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTDOWN);
				Sleep(50);
				::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTUP);
				Sleep(100);
				::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTDOWN);
				Sleep(50);
				::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTUP);
			}
			else if (key == 3) {
				pressed = true;
				::presskey(INPUT_MOUSE, MOUSEEVENTF_LEFTDOWN);
			}
			else if (key == 4) shifted = true;
			else if (key == -2);
			else {
				if (shifted) ::presskey(INPUT_KEYBOARD, 0, VK_SHIFT);
				if (lang == 0) {
					if (key == 'e') key = '0';
					else if (key == 't') key = '1';
					else if (key == 'a') key = '2';
					else if (key == 'o') key = '3';
					else if (key == 'i') key = '4';
					else if (key == 'n') key = '5';
					else if (key == 's') key = '6';
					else if (key == 'h') key = '7';
					else if (key == 'r') key = '8';
					else if (key == 'd') key = '9';
					else if (key == 'l') key = '.';
					else if (key == 'c') key = ',';
					else if (key == 'u') { shifted = true; ::presskey(INPUT_KEYBOARD, 0, VK_SHIFT); key = '/'; }
					else if (key == 'm') { shifted = true; ::presskey(INPUT_KEYBOARD, 0, VK_SHIFT); key = '1'; }
				}
				else if (lang == 1) {
					Hangul_On(0, false);
				}
				else if (lang == 2) {
					Hangul_On(0, true);
					if (key == 'e') key = 'd';
					else if (key == 't') key = 'k';
					else if (key == 'a') key = 's';
					else if (key == 'o') key = 'l';
					else if (key == 'i') key = 'r';
					else if (key == 'n') key = 'f';
					else if (key == 's') key = 'm';
					else if (key == 'h') key = 't';
					else if (key == 'r') key = 'j';
					else if (key == 'd') key = 'n';
					else if (key == 'l') key = 'e';
					else if (key == 'c') key = 'h';
					else if (key == 'u') key = 'w';
					else if (key == 'm') key = 'a';
					else if (key == 'w') key = 'g';
					else if (key == 'f') key = 'q';
					else if (key == 'y') key = 'o';
					else if (key == 'g') key = 'p';
					else if (key == 'p') key = 'u';
					else if (key == 'b') key = 'c';
					else if (key == 'v') key = 'x';
					else if (key == 'k') key = 'v';
					else if (key == 'x') key = 'b';
					else if (key == 'j') key = 'y';
					else if (key == 'z') key = 'i';
					else if (key == 'q') key = 'z';
				}
				if ('a' <= key && key <= 'z') key -= 0x20;
				else if (key == '\n') key = VK_RETURN;
				else if (key == '\b') key = VK_BACK;
				::presskey(INPUT_KEYBOARD, 0, key);
				Sleep(50);
				::presskey(INPUT_KEYBOARD, KEYEVENTF_KEYUP, key);
				if (shifted) {::presskey(INPUT_KEYBOARD, KEYEVENTF_KEYUP, VK_SHIFT); shifted = false; }
			}
		}
		Sleep(100);
	}
	
}
UINT CMFCApplication1Dlg::iothr(LPVOID x) {
	char buf[12];
	for (int i = 0; i < 12; i++) buf[i] = 0;
	int nbuf = 0;
	int lang = 1;
	int olang = -1;
	bool closed = false;
	unsigned long long cnt = 0;
	CMFCApplication1Dlg* pDialog = (CMFCApplication1Dlg*)x;
	const wchar_t* num = L"리모컨 명령을 사용하려면\n이 창을 누르고 치세요\n. -> 짧은(<0.4초) 깜빡임\n- -> 긴(0.4초-1.5초) 깜빡임\n+ -> 매우 긴(>1.5초) 깜빡임\n깜빡임 간격은 1초 이내로\n좌클릭 -.-\t우클릭 -...\n좌 더블클릭 --.\t좌 누른상태 --+\nbksp -.   \tspace -..\nenter -....\tshift ---..\n영어 ----\t한글 -+-\n숫자  -+..\n0eㅇ --..\t1tㅏ -.-.\n2aㄴ -..-\t3oㅣ ---\n4iㄱ -+   \t5nㄹ -.....\n6sㅡ -...-\t7hㅅ -..-.\n8rㅓ -.-..\t9dㅜ --...\n.lㄷ ---.   \t,cㅗ --.-\n?uㅈ -.--\t!mㅁ -.+\nwㅎ -+.\tfㅂ -.+.  \nyㅐ -....-\tgㅔ -...-.\npㅕ -..-..\tbㅊ -.-...\nvㅌ --....\tkㅍ -..--\nxㅠ -.-.-\tjㅛ --..-\nzㅑ -.--.  \tqㅋ --.-.\n";
	const wchar_t* eng = L"리모컨 명령을 사용하려면\n이 창을 누르고 치세요\n. -> 짧은(<0.4초) 깜빡임\n- -> 긴(0.4초-1.5초) 깜빡임\n+ -> 매우 긴(>1.5초) 깜빡임\n깜빡임 간격은 1초 이내로\n좌클릭 -.-\t우클릭 -...\n좌 더블클릭 --.\t좌 누른상태 --+\nbksp -.  \tspace -..\nenter -....\tshift ---..\n영어 ----\t한글 -+-\n숫자  -+..\na2ㄴ -..-\tbㅊ -.-...\nc,ㅗ --.-\td9ㅜ --...\ne0ㅇ --..\tfㅂ -.+.  \ngㅔ -...-.\th7ㅅ -..-.\ni4ㄱ -+  \tjㅛ --..-\nkㅍ -..--\tl.ㄷ ---.\nm!ㅁ -.+\tn5ㄹ -.....\no3ㅣ ---\tpㅕ -..-..\nqㅋ --.-.\tr8ㅓ -.-..\ns6ㅡ -...-\tt1ㅏ -.-.\nu?ㅈ -.--\tvㅌ --....\nwㅎ -+.\txㅠ -.-.-\nyㅐ -....-\tzㅑ -.--.\n";
	const wchar_t* kor = L"리모컨 명령을 사용하려면\n이 창을 누르고 치세요\n. -> 짧은(<0.4초) 깜빡임\n- -> 긴(0.4초-1.5초) 깜빡임\n+ -> 매우 긴(>1.5초) 깜빡임\n깜빡임 간격은 1초 이내로\n좌클릭 -.-\t우클릭 -...\n좌 더블클릭 --.\t좌 누른상태 --+\nbksp -.   \tspace -..\nenter -....\tshift ---..\n영어 ----\t한글 -+-\n숫자  -+..\nㄱ4i -+   \tㅏ1t -.-.\nㄴ2a -..-\tㅐy -....-\nㄷ.l ---.   \tㅑz -.--.\nㄹ5n -.....\tㅓ8r -.-..\nㅁ!m -.+\tㅔg -...-.\nㅂf -.+.  \tㅕp -..-..\nㅅ7h -..-.\tㅗ,c --.-\nㅇ0e --..\tㅛj --..-\nㅈ?u -.--\tㅜ9d --...\nㅊb -.-...\tㅠx -.-.-\nㅋq --.-.\tㅡ6s -...-\nㅌv --....\tㅣ3o ---\nㅍk -..--\tㅎw -+.\n";
	double r;
	int xt = 0;
	while (1) {
		cnt++;
		if (olang != lang) {
			olang = lang;
			if (lang == 0) ((CStatic*)(pDialog->GetDlgItem(IDC_list)))->SetWindowTextW(num);
			else if (lang == 1) ((CStatic*)(pDialog->GetDlgItem(IDC_list)))->SetWindowTextW(eng);
			else ((CStatic*)(pDialog->GetDlgItem(IDC_list)))->SetWindowTextW(kor);
		}
		if (pDialog->eye_updated2) {
			pDialog->eye_updated2 = false;
			r = pDialog->eye_r;
			if (r < 5 && closed) xt++;
			else xt = 0;
		}
		if (r > 8 && !closed) {
			closed = true;
			cnt = 0;
		}
		else if (r < 5 && closed&&xt>2) {
			xt = 0;
			closed = false;
			if (cnt < 40) buf[nbuf++] = '.';
			else if (cnt < 150) buf[nbuf++] = '-';
			else buf[nbuf++] = '+';
			if (nbuf == 10) nbuf = 9;
			cnt = 0;
		}
		if (closed) {
			if (cnt == 0) pDialog->beepf = 440;
			if (cnt == 40) pDialog->beepf = 550;
			if (cnt == 150) pDialog->beepf = 660;
		}
		else if (!closed && cnt > 100 && nbuf != 0) {
			buf[nbuf] = 0;
			nbuf = 0;
			cnt = 0;
			pDialog->currentlang = lang;
			if (strcmp(buf, "-.-") == 0) pDialog->currentkey=0;//좌클릭
			else if (strcmp(buf, "-...") == 0) pDialog->currentkey=1;//우클릭
			else if (strcmp(buf, "--.") == 0) pDialog->currentkey=2;//좌더블
			else if (strcmp(buf, "--+") == 0) pDialog->currentkey=3;//좌누른
			else if (strcmp(buf, "-.") == 0) pDialog->currentkey='\b';//백스페이스
			else if (strcmp(buf, "-..") == 0) pDialog->currentkey=' ';
			else if (strcmp(buf, "-....") == 0) pDialog->currentkey = '\n';
			else if (strcmp(buf, "---..") == 0) pDialog->currentkey = 4;//shift
			else if (strcmp(buf, "----") == 0) { pDialog->currentkey = -2, lang = 1; }
			else if (strcmp(buf, "-+-") == 0) { pDialog->currentkey = -2, lang = 2; }
			else if (strcmp(buf, "-+..") == 0) { pDialog->currentkey = -2, lang = 0; }
			else if (strcmp(buf, "--..") == 0) pDialog->currentkey='e';
			else if (strcmp(buf, "-.-.") == 0) pDialog->currentkey='t';
			else if (strcmp(buf, "-..-") == 0) pDialog->currentkey='a';
			else if (strcmp(buf, "---") == 0)  pDialog->currentkey='o';
			else if (strcmp(buf, "-+") == 0)   pDialog->currentkey='i';
			else if (strcmp(buf, "-.....") == 0) pDialog->currentkey='n';
			else if (strcmp(buf, "-...-") == 0)  pDialog->currentkey='s';
			else if (strcmp(buf, "-..-.") == 0)  pDialog->currentkey='h';
			else if (strcmp(buf, "-.-..") == 0)  pDialog->currentkey='r';
			else if (strcmp(buf, "--...") == 0)  pDialog->currentkey='d';
			else if (strcmp(buf, "---.") == 0)   pDialog->currentkey='l';
			else if (strcmp(buf, "--.-") == 0)   pDialog->currentkey='c';
			else if (strcmp(buf, "-.--") == 0)   pDialog->currentkey='u';
			else if (strcmp(buf, "-.+") == 0)    pDialog->currentkey='m';
			else if (strcmp(buf, "-+.") == 0)    pDialog->currentkey='w';
			else if (strcmp(buf, "-.+.") == 0)    pDialog->currentkey='f';
			else if (strcmp(buf, "-....-") == 0)  pDialog->currentkey='y';
			else if (strcmp(buf, "-...-.") == 0)  pDialog->currentkey='g';
			else if (strcmp(buf, "-..-..") == 0)  pDialog->currentkey='p';
			else if (strcmp(buf, "-.-...") == 0)  pDialog->currentkey='b';
			else if (strcmp(buf, "--....") == 0)  pDialog->currentkey='v';
			else if (strcmp(buf, "-..--") == 0)   pDialog->currentkey='k';
			else if (strcmp(buf, "-.-.-") == 0)   pDialog->currentkey='x';
			else if (strcmp(buf, "--..-") == 0)   pDialog->currentkey='j';
			else if (strcmp(buf, "-.--.") == 0)   pDialog->currentkey='z';
			else if (strcmp(buf, "--.-.") == 0)   pDialog->currentkey='q';
			for (int i = 0; i < 12; i++) buf[i] = 0;
		}
		Sleep(10); 
	}
	return 0;
}
UINT CMFCApplication1Dlg::beepthr(LPVOID x) {
	CMFCApplication1Dlg* pDialog = (CMFCApplication1Dlg*)x;
	while (1) {
		if (pDialog->beepf != 0) {
			int f = pDialog->beepf;
			pDialog->beepf = 0;
			::Beep(f, 50);
		}
		Sleep(10);
	}
}
UINT CMFCApplication1Dlg::processthr(LPVOID x) {
	CMFCApplication1Dlg* pDialog = (CMFCApplication1Dlg*)x;
	volatile double top,centerr,bottomr;//top:큼 right:큼
	volatile double left, right;
	volatile double centerx, centery, deltx, delty,deltr;
	double ix[75], iy[75],ir[75];
	int iptr = 0;
	int lastclosed = 0;
	int cnt = 0;
	bool init = false;
	bool lpressed = false;
	int prevs = -1;//1 up 2 down 3 left 4 right
	int prevg = 1;//2,4,8,16
	int prevgcnt = 0;//10->reset
	while (1) {
		if (pDialog->reset == 1) {
			Sleep(1000);
			for (int i = 0; i < 50; i++) {
				while (!pDialog->eye_updated || (pDialog->eye_x==-1&&pDialog->eye_y==-1)) Sleep(10);
				iy[i] = pDialog->eye_y; pDialog->eye_updated = false; Sleep(20);
			}
			sort(iy, iy + 50); top = iy[25];
			pDialog->reset = 2;
			Sleep(1000);
			for (int i = 0; i < 50; i++) {
				while (!pDialog->eye_updated || (pDialog->eye_x == -1 && pDialog->eye_y == -1)) Sleep(10);
				ir[i] = pDialog->eye_r; pDialog->eye_updated = false; Sleep(20);
			}
			sort(ir, ir + 50); bottomr = ir[25];
			pDialog->reset = 3;
			Sleep(1000);
			for (int i = 0; i < 50; i++) {
				while (!pDialog->eye_updated || (pDialog->eye_x == -1 && pDialog->eye_y == -1)) Sleep(10);
				ix[i] = pDialog->eye_x; pDialog->eye_updated = false; Sleep(20);
			}
			sort(ix, ix + 50); left = ix[25];
			pDialog->reset = 4;
			Sleep(1000);
			for (int i = 0; i < 50; i++) {
				while (!pDialog->eye_updated || (pDialog->eye_x == -1 && pDialog->eye_y == -1)) Sleep(10);
				ix[i] = pDialog->eye_x; pDialog->eye_updated = false; Sleep(20);
			}
			sort(ix, ix + 50); right = ix[25];
			pDialog->reset = 5;
			Sleep(1000);
			for (int i = 0; i < 50; i++) {
				while (!pDialog->eye_updated || (pDialog->eye_x == -1 && pDialog->eye_y == -1)) Sleep(10);
				iy[i] = pDialog->eye_y; ir[i] = pDialog->eye_r; pDialog->eye_updated = false; Sleep(20);
			}
			sort(iy, iy + 50); centery = iy[25]; sort(ir, ir + 50); centerr = ir[25];
			printf("%lf %lf %lf %lf %lf %lf\n", top, centery, left, right, centerr, bottomr);//0.127731 0.066172 -0.080230 -0.121973 3.928241 5.333333
			if (top > centery + 0.02 && left + 0.06 < right && centerr+.5<bottomr) {
				init = true;
				deltx = right - left; delty = top - centery; deltr = bottomr-centerr;
				centerx = (right + left) / 2; 
				pDialog->reset = -1;
			}
			else {
				pDialog->reset = 6;
				Sleep(2000);
				pDialog->reset = 1;
			}
		}
		else if(pDialog->reset == -1) {
			while (!pDialog->eye_updated || (pDialog->eye_x == -1 && pDialog->eye_y == -1)) Sleep(10);
			ix[iptr] = pDialog->eye_x; iy[iptr] = pDialog->eye_y; ir[iptr] = pDialog->eye_r; pDialog->eye_updated = false;
			iptr++;
			if (iptr == 15) iptr = 0;
			POINT mc; ::GetCursorPos(&mc);
			volatile double rx = centerx + ((double)mc.x/(double)pDialog->win_x-.5) * 0.7*deltx;
			volatile double ry= centery - ((double)mc.y / (double)pDialog->win_y - .5) * 1.4 * delty;
			volatile double rr=centerr;
			//커서 btm 큼 right 큼
			//top:큼 right:큼
			volatile int l = 0, r = 0, t = 0, b = 0;
			for (int i = 0; i < 15; i++) {
				if (rr + deltr * 0.7 < ir[i]) {
					if (b != -1) b++;
					//t = -1;
				}
				if(rx+deltx*0.4<ix[i]) {
					if (r != -1) r++;
					l = -1;
				}
				else if (rx - deltx * 0.4 > ix[i]) {
					if (l != -1) l++;
					r = -1;
				}
				if (ry + delty * 0.55 < iy[i]) {
					
					if (t != -1) t++;
					//b = -1;
				}
				
			}
			if (b > 10) {
				if (prevgcnt < 10 && prevg < 8 && prevs == 2) prevg *= 2;
				else  prevg = 1;
				int x = mc.x; int y = mc.y + prevg; if (y > pDialog->win_y) y = pDialog->win_y;
				::SetCursorPos(x, y);
				prevs = 2; prevgcnt = 0;
			}
			else if (l > 7) {
				if (prevgcnt < 10 && prevg < 8 && prevs == 3) prevg *= 2;
				else  prevg = 1;
				int x = mc.x - prevg; int y = mc.y; if (x < 0) x = 0;
				::SetCursorPos(x, y);
				prevs = 3; prevgcnt = 0;
			}
			else if (r > 7) {
				if (prevgcnt < 10 && prevg < 8 && prevs == 4) prevg *= 2;
				else  prevg = 1;
				int x = mc.x + prevg; int y = mc.y; if (x > pDialog->win_x) x = pDialog->win_x;
				::SetCursorPos(x, y);
				prevs = 4; prevgcnt = 0;
			}
			else if (t > 7) {
				if (prevgcnt < 10 && prevg < 8 && prevs == 1) prevg *= 2;
				else  prevg = 1;
				int x = mc.x; int y = mc.y - prevg; if (y < 0) y = 0;
				::SetCursorPos(x, y);
				prevs = 1; prevgcnt = 0;
			}
			
			prevgcnt++;
			
		}
		Sleep(20);
		cnt++; if (lastclosed == -1) cnt = 0;
	}
	return 0;
}

void CMFCApplication1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	printf("A");
	static bool sem = false;
	if (!sem) {
		printf("B");
		UpdateData(1);
		sem = true;
		capture->read(mat_frame);
		array2d<bgr_pixel> img;
		assign_image(img, cv_image<bgr_pixel>(mat_frame));
		pyramid_up(img);
		std::vector<dlib::rectangle> dets = detector(img);
		printf("E");
		if (dets.size() != 0) {
			printf("G");
			shape = sp(img, dets[0]);
			printf("H");
			detect(&img);
		}
		sem = false;
		printf("F");
		CClientDC dc(GetDlgItem(IDC_pic));
		CRect rect;
		GetDlgItem(IDC_pic)->GetClientRect(&rect);
		CDC memDC;
		CBitmap* pOldBitmap, bitmap;
		memDC.CreateCompatibleDC(&dc);
		bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		pOldBitmap = memDC.SelectObject(&bitmap);
		memDC.PatBlt(0, 0, rect.Width(), rect.Height(), BLACKNESS);
		GetDlgItem(IDC_pic)->GetClientRect(&rect);
		CImage m_img;
		m_img.Attach(Mat2DIB(&mat_frame));
		m_img.Draw(memDC.GetSafeHdc(), rect);
		GetDlgItem(IDC_pic)->GetClientRect(&rect);
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();
		bitmap.DeleteObject();
		UpdateData(0);
		printf("C");
	}
	printf("D");
	CDialogEx::OnTimer(nIDEvent);
}
#define max1(x,y) ((x>y)?x:y)
#define min1(x,y) ((x<y)?x:y)
bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
	double i = fabs(contourArea(cv::Mat(contour1)));
	double j = fabs(contourArea(cv::Mat(contour2)));
	return (i < j);
}
inline void tricolor(const Mat in,Mat& out) {
	int w = in.size().width;
	int h = in.size().height;
	static int tmp1[10000], tmp2[10000];
	int brp = 0;
	for (int i = 0; i < w; i++) {
		int blk = 0;
		for (int j = 0; j < h; j++) if (in.at<unsigned char>(j, i) < 50) blk++;
		if (blk > h / 4) {
			int ptr = 0; bool hh = false;
			for (int j = 0; j < h; j++) {
				bool br = (in.at<unsigned char>(j, i) < 50);
				if (br && !hh) { tmp1[ptr] = j; hh = true; }
				else if (!br && hh) { tmp2[ptr] = j; ptr++; hh = false;  }
			}
			if (hh) { tmp2[ptr] = h - 1; ptr++; hh = false; }
			int maxd = 0, maxi = 0;
			for (int j = 0; j < ptr; j++) {
				int diff = tmp2[j] - tmp1[j];
				if (maxd < diff) { maxd = diff; maxi = j; }
			}
			for (int j = tmp1[maxi]; j < tmp2[maxi]; j++) {
				if (0 <= j && j < h) {
					out.at<unsigned char>(j, i) = 0;
					brp++;
				}
			}
		}
	}
	int stackptr = 0;
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++) {
			if (out.at<unsigned char>(j,i)>130&& in.at<unsigned char>(j,i) > 200) {//row변경 세로선
				tmp1[stackptr] = i; tmp2[stackptr] = j; out.at<unsigned char>(j, i) = (char)255; stackptr++;
			}
		}
	while (stackptr) {
		stackptr--;
		int x = tmp1[stackptr]; int y = tmp2[stackptr];
		if (x <= 0 || y <= 0 || x >= w || y >= h) break;
		if (x > 0 && in.at<unsigned char>(y,x - 1) > 180 && out.at<unsigned char>(y,x-1)==210) {
			stackptr++;
			tmp1[stackptr] = x-1; tmp2[stackptr] = y; out.at<unsigned char>(y,x-1) = 255; 
		}
		if (x < w-1 && in.at<unsigned char>(y, x + 1) > 180 && out.at<unsigned char>(y, x + 1) == 210) {
			stackptr++;
			tmp1[stackptr] = x + 1; tmp2[stackptr] = y; out.at<unsigned char>(y,x + 1) = 255;
		}
		if (y > 0 && in.at<unsigned char>(y - 1,x) > 180 && out.at<unsigned char>(y - 1,x) == 210) {
			stackptr++;
			tmp1[stackptr] = x; tmp2[stackptr] = y - 1; out.at<unsigned char>(y - 1, x) = 255;
		}
		if (y < h-1 && in.at<unsigned char>(y + 1,x) > 180 && out.at<unsigned char>( y + 1,x) == 210) {
			stackptr++;
			tmp1[stackptr] = x; tmp2[stackptr] = y + 1; out.at<unsigned char>(y + 1, x) = 255; 
		}
		if (x > 0 && in.at<unsigned char>(y, x - 1) > 205 && out.at<unsigned char>(y, x - 1) == 127) {
			stackptr++;
			tmp1[stackptr] = x - 1; tmp2[stackptr] = y; out.at<unsigned char>(y, x - 1) = 255;
		}
		if (x < w - 1 && in.at<unsigned char>(y, x + 1) > 205 && out.at<unsigned char>(y, x + 1) == 127) {
			stackptr++;
			tmp1[stackptr] = x + 1; tmp2[stackptr] = y; out.at<unsigned char>(y, x + 1) = 255;
		}
		if (y > 0 && in.at<unsigned char>(y - 1, x) > 205 && out.at<unsigned char>(y - 1, x) == 127) {
			stackptr++;
			tmp1[stackptr] = x; tmp2[stackptr] = y - 1; out.at<unsigned char>(y - 1, x) = 255;
		}
		if (y < h - 1 && in.at<unsigned char>(y + 1, x) > 205 && out.at<unsigned char>(y + 1, x) == 127) {
			stackptr++;
			tmp1[stackptr] = x; tmp2[stackptr] = y + 1; out.at<unsigned char>(y + 1, x) = 255;
		}
	}
	for (int j = 0; j < h; j++) {
		bool f = false; int i = 0;
		while(1) {
			unsigned char n = out.at<unsigned char>(j, i);
			if (n == 255 || n == 0) break;
			else {i++; if (i == w) break;}
		}
		while (1) {
		X:;
			if (i != w) {
				while (1) {
					unsigned char n = out.at<unsigned char>(j, i);
					if (n == 255 || n == 0) i++;
					else break;
					if (i == w) goto X;
				}
				for (int k = i; k < w; k++) {
					unsigned char n = out.at<unsigned char>(j, k);
					if (n == 255 || n == 0) {
						for (int m = i; m < k; m++) {
							int bl = 0, wh = 0;
							if (j > 0) {
								if (m > 0) {
									unsigned char n = out.at<unsigned char>(j - 1, m-1);
									if (n == 0) bl++;
									else if (n == 255) wh++;
								}
								unsigned char n = out.at<unsigned char>(j - 1, m);
								if (n == 0) bl++;
								else if (n == 255) wh++;
								if (m <k-1) {
									unsigned char n = out.at<unsigned char>(j - 1, m + 1);
									if (n == 0) bl++;
									else if (n == 255) wh++;
								}
							}
							if (m > 0) {
								unsigned char n = out.at<unsigned char>(j, m - 1);
								if (n == 0) bl++;
								else if (n == 255) wh++;
							}
							if (m < k - 1) {
								unsigned char n = out.at<unsigned char>(j, m + 1);
								if (n == 0) bl++;
								else if (n == 255) wh++;
							}
							if (j < h - 1) {
								if (m > 0) {
									unsigned char n = out.at<unsigned char>(j + 1, m - 1);
									if (n == 0) bl++;
									else if (n == 255) wh++;
								}
								unsigned char n = out.at<unsigned char>(j + 1, m);
								if (n == 0) bl++;
								else if (n == 255) wh++;
								if (m < k - 1) {
									unsigned char n = out.at<unsigned char>(j + 1, m + 1);
									if (n == 0) bl++;
									else if (n == 255) wh++;
								}
							}
							if (bl > 5) out.at<unsigned char>(j, m) = 0;
							else out.at<unsigned char>(j, m) = 255;
							//color
						}
						i = k;
						goto X;
					}
				}
				break;
			}
			else break;
		}
		for (int a = 0; a < 2; a++)
			for (int i = 1; i < w - 1; i++) {
				for (int j = 1; j < h - 1; j++) {
					unsigned char n = out.at<unsigned char>(j, i);
					
					if (n != 0 && n != 255) {
						int bl = 0;
						if (out.at<unsigned char>(j - 1, i - 1) == 0) bl++;
						if (out.at<unsigned char>(j - 1, i) == 0) bl++;
						if (out.at<unsigned char>(j - 1, i + 1) == 0) bl++;
						if (out.at<unsigned char>(j, i - 1) == 0) bl++;
						if (out.at<unsigned char>(j, i + 1) == 0) bl++;
						if (out.at<unsigned char>(j + 1, i - 1) == 0) bl++;
						if (out.at<unsigned char>(j + 1, i) == 0) bl++;
						if (out.at<unsigned char>(j + 1, i + 1) == 0) bl++;
						if (bl > 5) out.at<unsigned char>(j, i) = 0;
						else if (bl > 4 && in.at<unsigned char>(j, i) < 120) out.at<unsigned char>(j, i) = 0;
						else if (bl > 3 && in.at<unsigned char>(j, i) < 85) out.at<unsigned char>(j, i) = 0;
						else if (bl > 2 && in.at<unsigned char>(j, i) < 75) out.at<unsigned char>(j, i) = 0;
						else if (bl > 1 && in.at<unsigned char>(j, i) < 65) out.at<unsigned char>(j, i) = 0;
						else if (bl > 0 && in.at<unsigned char>(j, i) < 50) out.at<unsigned char>(j, i) = 0;
						else {
							int wh = 0;
							if (out.at<unsigned char>(j - 1, i - 1) == 255) wh++;
							if (out.at<unsigned char>(j - 1, i) == 255) wh++;
							if (out.at<unsigned char>(j - 1, i + 1) == 255) wh++;
							if (out.at<unsigned char>(j, i - 1) == 255) wh++;
							if (out.at<unsigned char>(j, i + 1) == 255) wh++;
							if (out.at<unsigned char>(j + 1, i - 1) == 255) wh++;
							if (out.at<unsigned char>(j + 1, i) == 255) wh++;
							if (out.at<unsigned char>(j + 1, i + 1) == 255) wh++;
							if (wh > 5) out.at<unsigned char>(j, i) = 255;
						}
					}
					if (n == 255) {
						int bl = 0;
						if (out.at<unsigned char>(j - 1, i - 1) == 0) bl++;
						if (out.at<unsigned char>(j - 1, i) == 0) bl++;
						if (out.at<unsigned char>(j - 1, i + 1) == 0) bl++;
						if (out.at<unsigned char>(j, i - 1) == 0) bl++;
						if (out.at<unsigned char>(j, i + 1) == 0) bl++;
						if (out.at<unsigned char>(j + 1, i - 1) == 0) bl++;
						if (out.at<unsigned char>(j + 1, i) == 0) bl++;
						if (out.at<unsigned char>(j + 1, i + 1) == 0) bl++;
						if (bl > 5) out.at<unsigned char>(j, i) = 0;
					}
				}
			}
		
	}
	int min_x = -1, max_x = -1, min_y = -1, max_y = -1, min_x2 = -1, max_x2 = -1;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			if (out.at<unsigned char>(j, i) == 0) {
				if (min_y == -1) {
					min_y = j; max_y = j;
				}
				else {
					if (min_y > j) min_y = j;
					else if (max_y < j) max_y = j;
				}
			}
		}
	}
	for (int i = 0; i < w; i++) {
		int th = 0;
		for (int j = 0; j < h; j++) {
			if (out.at<unsigned char>(j, i) == 0) {
				th++;
			}
		}
		if (th&&th < ((max_y - min_y) *2)/5) {
			for (volatile int j = 0; j < h; j++) {
				if (out.at<unsigned char>(j, i) == 0) {
					out.at<unsigned char>(j, i) = 255;
				}
			}
		}
	}
	min_y = -1, max_y = -1;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			if (out.at<unsigned char>(j, i) == 0) {
				if (min_x == -1) {
					min_x = i; max_x = i; min_y = j; max_y = j;
				}
				else {
					max_x = i;
					if (min_y > j) min_y = j;
					else if (max_y < j) max_y = j;
				}
			}
			if (out.at<unsigned char>(j, i) != 127) {
				if (min_x2 == -1) {
					min_x2 = i; max_x2 = i; 
				}
				else max_x2 = i;
			}
		}
	}
	if (max_x - min_x > (max_y - min_y)*4/3) {
		int d1 = (min_x2 < min_x ? min_x - min_x2 : min_x2 - min_x);
		int d2= (max_x2 < max_x ? max_x - max_x2 : max_x2 - max_x);
		if (d1<d2) {
			int t1 = max_x - (max_y - min_y) * 4 / 3+2;
			if (t1 < 0) t1 = 0;
			int t2 = t1 - 2;
			if (t2 < 0) t2 = 0;
			for (int i = 0; i < t2; i++) {
				for (int j = 0; j < h; j++) if(out.at<unsigned char>(j,i)==0) out.at<unsigned char>(j, i) = 255;
			}
			for (int i = t2; i < t1; i++) {
				for (int j = 0; j < h; j++) 
					if (out.at<unsigned char>(j, i) == 0) {
						if (in.at<unsigned char>(j, i) < 30) out.at<unsigned char>(j, i) = 0;
						else out.at<unsigned char>(j, i) = 255;
					}
			}
		}
		else {
			int t1 = min_x + (max_y - min_y) * 4 / 3-2;
			if (t1 < 0) t1 = 0;
			int t2 = t1 + 2;
			if (t2 < 0) t2 = 0;
			for (int i = t2; i < w; i++) {
				for (int j = 0; j < h; j++) if (out.at<unsigned char>(j, i) == 0) out.at<unsigned char>(j, i) = 255;
			}
			for (int i = t1; i < t2; i++) {
				for (int j = 0; j < h; j++) 
					if (out.at<unsigned char>(j, i) == 0) {
						if (in.at<unsigned char>(j, i) < 30) out.at<unsigned char>(j, i) = 0;
						else out.at<unsigned char>(j, i) = 255;
					}
			}
		}
	}
}
inline double removeoutlier(double x1, double x2, double x3, double x4) {
	double ret = (x1 + x2 + x3 + x4) / 4.;
	double dx1 = (x1 > ret ? x1 - ret : ret - x1);
	double dx2 = (x2 > ret ? x2 - ret : ret - x2);
	double dx3 = (x3 > ret ? x3 - ret : ret - x3);
	double dx4 = (x4 > ret ? x4 - ret : ret - x4);
	if (dx1 > dx2 && dx1 > dx3 && dx1 > dx4) return (x2 + x3 + x4) / 3.;
	else if (dx2 > dx1 && dx2 > dx3 && dx2 > dx4) return (x1 + x3 + x4) / 3.;
	else if (dx3 > dx1 && dx3 > dx2 && dx3 > dx4) return (x1 + x2 + x4) / 3.;
	else return (x1 + x2 + x3) / 3.;
}
inline void calculateratio(const Mat left, const Mat right, double& x, double& y,double& r) {
	int lh = left.size().height, lw = left.size().width, rh = right.size().height, rw = right.size().width;
	r = (((double)lw / (double)lh) + ((double)rw / (double)rh)) / 2.;
	if (r > 8.5) {
		x = -1; y = -1; return;
	}
	int ax = 0, ay = 0, bx = 0, by = 0;
	int at = 0, bt = 0;
	int bcnt = 0;
	for (int i = 0; i < lw; i++)
		for (int j = 0; j < lh; j++) {
			unsigned char t = left.at<unsigned char>(j, i);
			if (t == 0) {
				at++; ax += i; ay += j; bt++; bx += i; by += j; bcnt++;
			}
			else if (t >130) {
				at++; ax += i; ay += j;
			}
		}
	double lx = ((double)ax / (double)at- (double)bx / (double)bt)/(double)lw;
	double ly = ((double)ay / (double)at- (double)by / (double)bt)/(double)lh;
	ax = 0, ay = 0, bx = 0, by = 0;
	at = 0, bt = 0;
	for (int i = 0; i < rw; i++)
		for (int j = 0; j < rh; j++) {
			unsigned char t = right.at<unsigned char>(j, i);
			if (t == 0) {
				at++; ax += i; ay += j; bt++; bx += i; by += j; bcnt++;
			}
			else if (t > 130) {
				at++; ax += i; ay += j;
			}
		}
	if (bcnt < (lw*lh+rw*rh)/20) {
		x = -1; y = -1; r = 10; return;
	}
	double rx = ((double)ax / (double)at - (double)bx / (double)bt)/(double)rw;
	double ry = ((double)ay / (double)at - (double)by / (double)bt)/(double)rh;
	double tx4, ty4, tr4;
	static double tx1 = 0, tx2 = 0, tx3 = 0, ty1 = 0, ty2 = 0, ty3 = 0, tr1 = 0, tr2 = 0, tr3 = -1;
	tx4 = (lx + rx) / 2., ty4 = (ly + ry) / 2; tr4 = (((double)lw / (double)lh) + ((double)rw / (double)rh)) / 2.;
	if (tr3 == -1) {
		tr3 = tr4; tr2 = tr3; tr1 = tr2;
	}
	if (isnan(tx4) || isnan(ty4)) { tx4 = tx3; ty4 = ty3; }
	x = removeoutlier(tx1, tx2, tx3, tx4); y = removeoutlier(ty1, ty2, ty3, ty4); r = removeoutlier(tr1, tr2, tr3, tr4);
	tx1 = tx2; tx2 = tx3; tx3 = tx4; ty1 = ty2; ty2 = ty3; ty3 = ty4; tr1 = tr2; tr2 = tr3; tr3 = tr4;
}
inline void printimage(Mat& target, const Mat left, const Mat right, const Mat left2, const Mat right2) {
	int lh = left.size().height, lw = left.size().width, rh = right.size().height, rw = right.size().width;
	for (int i = 0; i < lw; i++)
		for (int j = 0; j < lh; j++) {
			target.at<Vec3b>(j, i)[0] = left.at<unsigned char>(j, i);
			target.at<Vec3b>(j, i)[1] = (left2.at<unsigned char>(j, i)>>1)+ (left.at<unsigned char>(j, i)>>1);
			target.at<Vec3b>(j, i)[2] = left2.at<unsigned char>(j, i);
		}
	for (int i = 0; i < rw; i++)
		for (int j = 0; j < rh; j++) {
			target.at<Vec3b>(j, i + lw)[0] = right.at<unsigned char>(j, i);
			target.at<Vec3b>(j, i + lw)[1] = (right2.at<unsigned char>(j, i)>>1)+ (right.at<unsigned char>(j, i) >> 1);
			target.at<Vec3b>(j, i + lw)[2] = right2.at<unsigned char>(j, i);
		}
}
inline void printimage(Mat& target, const Mat left, const Mat right) { printimage(target, left, right, left, right); }
int CMFCApplication1Dlg::detect(array2d<bgr_pixel>* img) {
	int ret = 0;
	dlib::point ltl = shape.part(37),ltr = shape.part(38),ll = shape.part(36),lr = shape.part(39),lbl = shape.part(41),lbr = shape.part(40),
		rtl = shape.part(43),rtr = shape.part(44),rl = shape.part(42),rr = shape.part(45),rbl = shape.part(47),rbr = shape.part(46);
	double divx = (double)mat_frame.size().width / (double)img->nc();
	double divy= (double)mat_frame.size().height / (double)img->nr();
	int lty = (int)((min1(ltl.y(), ltr.y()))* divy);
	int lby = (int)((max1(lbl.y(), lbr.y())) * divy);
	int lrx = (int)((lr.x())*divx);
	int llx = (int)((ll.x())*divx);
	int rty = (int)((min1(rtl.y(), rtr.y())) * divy);
	int rby = (int)((max1(rbl.y(), rbr.y())) * divy);
	int rrx = (int)((rr.x()) * divx);
	int rlx = (int)((rl.x()) * divx);
	int ly = lby - lty; ly >>=2; lty -= ly; lby += ly;
	int lx = lrx - llx; lx >>= 2; llx -= lx; lrx += lx;
	int ry = rby - rty; ry >>= 2; rty -= ry; rby += ry;
	int rx = rrx - rlx; rx >>= 2; rlx -= rx; rrx += rx;
	try {
		Mat l = mat_frame(Rect(llx, lty, lrx - llx, lby - lty));
		Mat r = mat_frame(Rect(rlx, rty, rrx - rlx, rby - rty));
		Mat t(mat_frame.size(), CV_8U, Scalar(127));
		std::vector<Point> db1 = { Point(ltl.x() * divx,ltl.y() * divy), Point(ltr.x() * divx,ltr.y() * divy), Point(lr.x() * divx,lr.y() * divy),
		Point(lbr.x() * divx,lbr.y() * divy), Point(lbl.x() * divx,lbl.y() * divy), Point(ll.x() * divx,ll.y() * divy) };
		std::vector<Point> db2 = { Point(rtl.x() * divx,rtl.y() * divy), Point(rtr.x() * divx,rtr.y() * divy), Point(rr.x() * divx,rr.y() * divy),
		Point(rbr.x() * divx,rbr.y() * divy), Point(rbl.x() * divx,rbl.y() * divy), Point(rl.x() * divx,rl.y() * divy) };
		fillPoly(t, db1, Scalar(210)); fillPoly(t, db2, Scalar(210));
		Mat gl, gr;
		cvtColor(l, gl, cv::COLOR_BGR2GRAY);
		cvtColor(r, gr, cv::COLOR_BGR2GRAY);
		Mat ngl, ngr;
		normalize(gl, ngl, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		normalize(gr, ngr, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		Mat ngl2, ngr2;
		bilateralFilter(ngl, ngl2, 10, 15, 15);
		bilateralFilter(ngr, ngr2, 10, 15, 15);
		Mat ngl3 = t(Rect(llx, lty, lrx - llx, lby - lty));
		Mat ngr3 = t(Rect(rlx, rty, rrx - rlx, rby - rty));
		tricolor(ngl2,ngl3);
		tricolor(ngr2,ngr3);
		calculateratio(ngl3, ngr3, eye_x, eye_y,eye_r);
		printimage(mat_frame,ngl2,ngr2,ngl3, ngr3);
		eye_updated = true; eye_updated2 = true;
		printf("%lf %lf %lf\n", eye_x, eye_y,eye_r);
	}
	catch (Exception e) { printf("%s\n", e.what()); }
	
	return ret;
}

int CMFCApplication1Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

BOOL CMFCApplication1Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_CHAR)
	{
		this->SendMessage(WM_CHAR, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMFCApplication1Dlg::OnClickedReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(reset<0) reset = 1;
}

void CMFCApplication1Dlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	static bool sem = false;
	if ('0' <= (char)nChar && (char)nChar <= '9') {
		((Serial*)serial)->WriteData("r", 1);
		Sleep(1000);
		((Serial*)serial)->ReadData(hwbuf[nChar-'0'], 1602);
	}
	else if ('a' <= (char)nChar&&(char)nChar<='j') {
		if (sem == false) {
			sem = true;
			((Serial*)serial)->WriteData(hwbuf[nChar-'a'], 1602);
			Sleep(1000);
			sem = false;
		}
	}
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
	
}
