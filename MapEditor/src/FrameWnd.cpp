////////////////////////////////////////////////////////////////////////////////
//
//                        ����ܴ���ģ��
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		FrameWnd.cpp
//
//	˵����
//
//
//
//////////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "msimg32.lib")

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"

#include "Menu.h"
#include "ToolBar.h"
#include "ImageSelWnd.h"
#include "Document.h"
#include "MapEditWnd.h"
#include "TemplateEditWnd.h"
#include "MiniMapWnd.h"
#include "TileAttributeWnd.h"
#include "StatusBar.h"

#include "FrameWnd.h"

extern CStatusBar	g_StatusBar;

// ģ��ճ��ģʽ
BOOL			g_bAITemplatePaster = TRUE;

////////////////////////////////////////////////////////////////////////////////
// �ڲ�����
////////////////////////////////////////////////////////////////////////////////
BOOL	FW_bShowToolBar			= TRUE;
BOOL	FW_bShowStatusBar		= TRUE;
BOOL	FW_bShowMiniMapWnd		= TRUE;
BOOL	FW_bShowTileAttributeWnd	= TRUE;
BOOL	FW_bShowImageSelWnd		= TRUE;

HWND	FW_hFrameWnd		= NULL;
HWND	FW_hMDIClientWnd	= NULL;

DWORD	ID_MDICLIENTWND		= 202;

BOOL	FW_bOpenMapEditWnd		= FALSE;
BOOL	FW_bOpenTemplateEditWnd		= FALSE;

// ������ʾ
const CHAR	FRAMEWNDCLASS[]		= "FrameWndClass";

////////////////////////////////////////////////////////////////////////////////
//
//                       ��������
//
//	FrameWnd_OnCreate
//	FrameWnd_OnCommand
//	FrameWnd_OnNotify
//	FrameWnd_OnSize
//
//	��Щ������FrameWnd_WindowProc����
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK FrameWnd_WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT FrameWnd_OnCreate(HWND hWnd);
LRESULT FrameWnd_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT FrameWnd_OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT FrameWnd_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT FrameWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT FrameWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID FrameWnd_OnFileOpen(HWND hWnd);

////////////////////////////////////////////////////////////////////////////////
//
//                        ��������
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL FrameWnd_Create(HINSTANCE hInstance)
//
// ����  :
//	����������
//
// ����ֵ:
//	
// ����  :
//	HINSTANCE hInstance -
//
////////////////////////////////////////////////////////////////////////////////
BOOL FrameWnd_Create(HINSTANCE hInstance)
{
	ATOM atom;
	WNDCLASSEX stWndClass;

	////////////////////////////////////////////////////////////////////////////////
	// ע������ܴ�����
	////////////////////////////////////////////////////////////////////////////////
	stWndClass.cbSize = sizeof(WNDCLASSEX);
	stWndClass.style = CS_HREDRAW | CS_VREDRAW;
	stWndClass.lpfnWndProc = FrameWnd_WindowProc;
	stWndClass.cbClsExtra = 0;
	stWndClass.cbWndExtra = 0;
	stWndClass.hInstance = hInstance;
	stWndClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	stWndClass.hCursor = ::LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
	stWndClass.hbrBackground = ::GetSysColorBrush(COLOR_BTNFACE);
	stWndClass.lpszMenuName = NULL;
	stWndClass.lpszClassName = FRAMEWNDCLASS;
	stWndClass.hIconSm = NULL;

	atom = RegisterClassEx(&stWndClass);
	if (atom == 0)
	{
		return FALSE;
	}

	FW_hFrameWnd = ::CreateWindowEx(
		0,
		FRAMEWNDCLASS,
		"�ɽ�������һ����ͼ�༭��",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (FW_hFrameWnd == NULL)
	{
		return FALSE;
	}

	::ShowWindow(FW_hFrameWnd, SW_MAXIMIZE);
	::UpdateWindow(FW_hFrameWnd);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	LRESULT CALLBACK FrameWnd_WindowProc(
//				    HWND hWnd,
//				    UINT uMsg,
//				    WPARAM wParam,
//				    LPARAM lParam)
//
// ����  :
//	����ܴ�����Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	uMsg -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK FrameWnd_WindowProc(
				    HWND hWnd,
				    UINT uMsg,
				    WPARAM wParam,
				    LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_CREATE:
		return FrameWnd_OnCreate(hWnd);
		break;
	case WM_CLOSE:
		return FrameWnd_OnClose(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		return FrameWnd_OnDestroy(hWnd, wParam, lParam);
		break;
	case WM_SIZE:
		return FrameWnd_OnSize(hWnd, wParam, lParam);
	case WM_COMMAND:
		return FrameWnd_OnCommand(hWnd, wParam, lParam);
		break;
	case WM_NOTIFY:
		return FrameWnd_OnNotify(hWnd, wParam, lParam);
		break;
	default:
		return ::DefFrameProc(hWnd, FW_hMDIClientWnd, uMsg, wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID FrameWnd_OnCreate(HWND hWnd)
//
// ����  :
//	WM_CREATE��Ϣ������
//	����ͣ�����ڡ���������״̬����ͼ��ѡ�񴰿ڡ����ĵ�����ͻ�����
//
// ����ֵ:
//	
// ����  :
//	HWND hWnd -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT FrameWnd_OnCreate(HWND hWnd)
{
	////////////////////////////////////////////////////////////////////////////////
	// �����˵�
	////////////////////////////////////////////////////////////////////////////////
	if (!Menu_Create(hWnd))
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ����������
	////////////////////////////////////////////////////////////////////////////////
	if (!ToolBar_Create(hWnd))
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ����״̬��
	////////////////////////////////////////////////////////////////////////////////
	if (!g_StatusBar.Create(hWnd))
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ����ͼ��ѡ�񴰿�
	////////////////////////////////////////////////////////////////////////////////
	if (!ImageSelWnd_Create(hWnd))
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// �������ĵ�����ͻ�����
	////////////////////////////////////////////////////////////////////////////////
	HINSTANCE hInstance	= NULL;
	CLIENTCREATESTRUCT ClientCreate;

	::memset(&ClientCreate, 0, sizeof(CLIENTCREATESTRUCT));

	hInstance = (HINSTANCE)::GetWindowLong(hWnd, GWL_HINSTANCE);

	FW_hMDIClientWnd = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"MDICLIENT",
		NULL,
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		0,
		0,
		0,
		0,
		hWnd,
		(HMENU)ID_MDICLIENTWND,
		hInstance,
		&ClientCreate
		);

	if (FW_hMDIClientWnd == NULL)
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ����С��ͼ����
	////////////////////////////////////////////////////////////////////////////////
	if (!MiniMapWnd_Create(hWnd))
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ����Tile���Դ���
	////////////////////////////////////////////////////////////////////////////////
	if (!TileAttributeWnd_Create(hWnd))
	{
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ��ʼ���ĵ�
	////////////////////////////////////////////////////////////////////////////////
	Document_Init();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID FrameWnd_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_COMMAND��Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT FrameWnd_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDCMD_FILE_OPEN:
		FrameWnd_OnFileOpen(hWnd);
		break;
	case IDCMD_FILE_SAVE:
		Document_SaveFile();
		break;

	case IDCMD_APP_EXIT:
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case IDCMD_APP_ABOUT:
		break;

	case IDCMD_TEMPLATE_CLEAR:
		TemplateEditWnd_ClearTemplate();
		break;
	case IDCMD_TEMPLATE_AIPASTER:
		g_bAITemplatePaster = !g_bAITemplatePaster;
		Menu_Check(IDCMD_TEMPLATE_AIPASTER, g_bAITemplatePaster);
		break;

	case IDCMD_VIEW_MINIMAPWND:
		{
			int nShow	= 0;
			FW_bShowMiniMapWnd = !FW_bShowMiniMapWnd;
			nShow = FW_bShowMiniMapWnd ? SW_SHOW : SW_HIDE;
			MiniMapWnd_Show(nShow);
			Menu_Check(IDCMD_VIEW_MINIMAPWND, FW_bShowMiniMapWnd);
		}
		break;
	case IDCMD_VIEW_TILEATTRIBUTEWND:
		{
			int nShow	= 0;
			FW_bShowTileAttributeWnd = !FW_bShowTileAttributeWnd;
			nShow = FW_bShowTileAttributeWnd ? SW_SHOW : SW_HIDE;
			TileAttributeWnd_Show(nShow);
			Menu_Check(IDCMD_VIEW_TILEATTRIBUTEWND, FW_bShowTileAttributeWnd);
		}
		break;
	case IDCMD_WINDOW_MAPEDITWND:
		MapEditWnd_Show(SW_SHOWMAXIMIZED);
		break;
	case IDCMD_WINDOW_TEMPLATEEDITWND:
		TemplateEditWnd_Show(SW_SHOWMAXIMIZED);
		break;
	default:
		// ������Ϣ(IDCMD_EDIT_...)������ͼ���ڻ�ģ�崰��
		HWND hWndActive = NULL;
		hWndActive = (HWND)::SendMessage(FW_hMDIClientWnd, WM_MDIGETACTIVE, 0, 0);
		if (::IsWindow(hWndActive))
		{
			::SendMessage(hWndActive, WM_COMMAND, wParam, lParam);
		}
	}
	return ::DefFrameProc(hWnd, FW_hMDIClientWnd, WM_COMMAND, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID FrameWnd_OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_NOTIFY��Ϣ������
//
// ����ֵ:
//	
// ����  :
//	lParam -
//	lParam -
//
///////////////////////////////////////////////////////////////////////////////
LRESULT FrameWnd_OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPNMTTDISPINFO lpNMTT = (LPNMTTDISPINFO)lParam;

	switch (lpNMTT->hdr.code)
	{
	case TTN_GETDISPINFO:
		{
			lpNMTT->lpszText = MAKEINTRESOURCE(lpNMTT->hdr.idFrom);
			break; 
		}
		break;
	}

	return ::DefFrameProc(hWnd, FW_hMDIClientWnd, WM_NOTIFY, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID FrameWnd_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_SIZE��Ϣ������
//	�������Ӵ��ڵĲ���
//
// ����ֵ:
//	
// ����  :
//	Wnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT FrameWnd_OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT	rc;
	POINT	pt;

	// �ܵĿͻ����ռ�
	::GetClientRect(hWnd, &rc);

	ToolBar_SizeParent(&rc);
	g_StatusBar.SizeParent(&rc);

	ImageSelWnd_SizeParent(&rc);


	// ʣ�µĿռ�͸����ĵ�����ͻ�����
	::MoveWindow(FW_hMDIClientWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

	// �ƶ�С��ͼ
	pt.x = rc.right - 128 - 6 - 2 - 16;
	pt.y = rc.bottom - 128 - 3 - 19 - 16 - 2;
	::ClientToScreen(hWnd, &pt);
	MiniMapWnd_Move(pt.x, pt.y);

	// �ƶ�Tile���Դ���
	pt.x = rc.right - 84 - 6 - 2 - 16;
	pt.y = rc.top + 2;
	::ClientToScreen(hWnd, &pt);

	TileAttributeWnd_Move(pt.x, pt.y);
	return 0;
}

LRESULT FrameWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (!Document_CloseFile(hWnd))
	{
		return 0;
	}
	else
	{
		return ::DefFrameProc(hWnd, FW_hMDIClientWnd, WM_CLOSE, wParam, lParam);
	}
}

LRESULT FrameWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	Document_Release();
	::PostQuitMessage(0);
	return ::DefFrameProc(hWnd, FW_hMDIClientWnd, WM_DESTROY, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID FrameWnd_OnFileOpen(HWND hWnd)
//
// ����  :
//	
// ����ֵ:
//	
// ����  :
//	HWND hWnd -
//
////////////////////////////////////////////////////////////////////////////////
VOID FrameWnd_OnFileOpen(HWND hWnd)
{
	CHAR	Buffer[ MAX_PATH ];

	if (Document_OpenFile(hWnd))
	{
		// ����ͼ��ѡ�񴰿ڵ�ͼ���б�
		ImageSelWnd_Update();

		// �ػ�С��ͼ
		MiniMapWnd_Paint();

		if (FW_bOpenTemplateEditWnd == FALSE)
		{
			if (TemplateEditWnd_Create(FW_hMDIClientWnd))
			{
				FW_bOpenTemplateEditWnd = TRUE;
			}
		}

		if (FW_bOpenMapEditWnd == FALSE)
		{
			if (MapEditWnd_Create(FW_hMDIClientWnd))
			{
				FW_bOpenMapEditWnd = TRUE;
			}
		}
	
		sprintf(Buffer, "��ͼ - %s", Document_GetFileName());
		MapEditWnd_SetTitle(Buffer);

		sprintf(Buffer, "ģ�� - %s", Document_GetFileName());
		TemplateEditWnd_SetTitle(Buffer);

		TemplateEditWnd_ClearTemplate();
	}
}