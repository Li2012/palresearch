////////////////////////////////////////////////////////////////////////////////
//
//                              
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		WinMain.cpp
//
//	˵����
//
//
//	������
//		
//
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"

#include "Map.h"
#include "FrameWnd.h"
#include "StatusBar.h"
#include "Palette.h"


#include "WinMain.h"

////////////////////////////////////////////////////////////////////////////////
//
//                              ȫ�ֱ���
//
////////////////////////////////////////////////////////////////////////////////

// ��ͼ����
WORD		g_MapData[ 128 ][ 128 ][ 2 ];

// ģ������
WORD		g_TemplateData[ 128 ][ 128 ][ 2 ];

// ͼ��ͼ��
DWORD		g_dwImageCount	= 0;
HDC		g_hDCTileImage[ 512 ];// ͼ��
HDC		g_hDCMiniTileImage[ 512 ]; // ���������ͼ

// ��ɫ��
RGBQUAD		g_Palette[ 256 ];		// window�ĵ�ɫ��ṹ

DWORD		g_dwTemplateCount;
DRAWTILESTRUCT	g_Template[ MAX_TEMPLATE ];

// ״̬��
CStatusBar	g_StatusBar;

////////////////////////////////////////////////////////////////////////////////
//
//
//                              ��������
//
//
////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
BOOL	InitApplication(HINSTANCE hInstance);	// ��ʼ������
BOOL	InitInstance(HINSTANCE hInstance);	// ��ʼ��ʵ��
void	ExitApplication(void);			// �˳�����
int	Run(void);				// ������Ϣѭ��


////////////////////////////////////////////////////////////////////////////////
//
//
//                              ��������
//
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//
// ����  :
//	Win32Ӧ�ó������
//
// ����ֵ:
//	
// ����  :
//	hInstance -
//	hPrevInstance -
//	lpCmdLine -
//	nCmdShow -
//
////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!InitApplication(hInstance) || !InitInstance(hInstance))
		return 0;

	Run();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL InitApplication(HINSTANCE hInstance)
//
// ����  :
//	��ʼ��Ӧ�ó���ע�ᴰ����
//	
// ����ֵ:
//	
// ����  :
//	HINSTANCE hInstance -
//
////////////////////////////////////////////////////////////////////////////////
BOOL InitApplication(HINSTANCE hInstance)
{
	// ע�ṫ���ؼ���
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL InitInstance(HINSTANCE hInstance)
//
// ����  :
//	��ʼ��ʵ��������������
//
// ����ֵ:
//	
// ����  :
//	HINSTANCE hInstance -
//
////////////////////////////////////////////////////////////////////////////////
BOOL InitInstance(HINSTANCE hInstance)
{
//	m_hAccelerators = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	// ����ɫ��
	if (!Palette_Init(g_Palette))
	{
		return FALSE;
	}


	if (!FrameWnd_Create(hInstance))
	{
		return FALSE;
	}

	return TRUE;
}

// �˳�����
void ExitApplication(void)
{

}

//      ���г��򣬽�����Ϣѭ��
int Run(void)
{
	MSG msg;
	while (::GetMessage(&msg, (HWND)NULL, 0, 0))
	{
/*		if (!TranslateAccelerator(
			m_pMainWnd->GetWindow(),           // handle to receiving window
			m_hAccelerators,    // handle to active accelerator table
			&msg))              // address of message data
*/		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}