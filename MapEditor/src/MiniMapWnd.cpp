////////////////////////////////////////////////////////////////////////////////
//
//                              С��ͼ
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		MiniMapWnd.cpp
//
//	˵����
//		128 x 128���أ�1�����ش���һ��Tile
//
//	������
//		
//
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>

#include "Map.h"
#include "MapEx.h"
#include "MapEditWnd.h"
#include "TemplateEditWnd.h"

#include "MiniMapWnd.h"

// ����WinMain.cpp
extern WORD		g_MapData[ 128 ][ 128 ][ 2 ];
extern HDC		g_hDCMiniTileImage[ 512 ];


HWND		MMW_hMiniMapWnd		= NULL;
HDC		MMW_hDCMiniMap		= NULL;

// �����MapEditWnd.cpp��MEW_lxCamera��MEW_lyCamera��ͬ
SHORT		MMW_nx			= 0;
SHORT		MMW_ny			= 0;

const LPCSTR	MINIMAPWNDCLASS		= "MiniMapWndClass";

////////////////////////////////////////////////////////////////////////////////
//
//                              ��������
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MiniMapWnd_WindowProc(
				       HWND hWnd,
				       UINT uMsg,
				       WPARAM wParam,
				       LPARAM lParam);

LRESULT MiniMapWnd_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT MiniMapWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT MiniMapWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT MiniMapWnd_OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT MiniMapWnd_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

LRESULT MiniMapWnd_OnNcActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID MiniMapWnd_DrawSelectRect();

////////////////////////////////////////////////////////////////////////////////
//
//                              ��������
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL MiniMapWnd_Create(HWND hWndParent)
//
// ����  :
//	����С��ͼ��������
//
// ����ֵ:
//	
// ����  :
//	HWND hWndParent -
//
////////////////////////////////////////////////////////////////////////////////
BOOL MiniMapWnd_Create(HWND hWndParent)
{
	HINSTANCE hInstance	= NULL;

	ATOM atom;
	WNDCLASSEX stWndClass;

	hInstance = (HINSTANCE)::GetWindowLong(hWndParent, GWL_HINSTANCE);
	////////////////////////////////////////////////////////////////////////////////
	// ע��С��ͼ����������
	////////////////////////////////////////////////////////////////////////////////
	stWndClass.cbSize = sizeof(WNDCLASSEX);
	stWndClass.style = CS_HREDRAW | CS_VREDRAW;
	stWndClass.lpfnWndProc = MiniMapWnd_WindowProc;
	stWndClass.cbClsExtra = 0;
	stWndClass.cbWndExtra = 0;
	stWndClass.hInstance = hInstance;
	stWndClass.hIcon = NULL;
	stWndClass.hCursor = ::LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
	stWndClass.hbrBackground = ::GetSysColorBrush(COLOR_BTNFACE);
	stWndClass.lpszMenuName = NULL;
	stWndClass.lpszClassName = MINIMAPWNDCLASS;
	stWndClass.hIconSm = NULL;

	// ע�ḡ��������
	atom = RegisterClassEx(&stWndClass);
	if (atom == 0)
	{
		return FALSE;
	}

	MMW_hMiniMapWnd = ::CreateWindowEx(
		0x180,
		MINIMAPWNDCLASS,
		"С��ͼ",
		WS_CLIPSIBLINGS | WS_DLGFRAME |
		WS_OVERLAPPED |
		0x3B00 | WS_POPUPWINDOW |
		WS_CAPTION | WS_VISIBLE,
		600, 200, 134, 150,
		hWndParent,
		NULL,
		hInstance,
		NULL);

	if (MMW_hMiniMapWnd == NULL)
	{
		::MessageBox(hWndParent, "����С��ͼ����ʧ�ܣ�", NULL, MB_OK);
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
//  MiniMapWnd_WindowProc
//
//      ������Ϣ������
//
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MiniMapWnd_WindowProc(
				       HWND hWnd,
				       UINT uMsg,
				       WPARAM wParam,
				       LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return MiniMapWnd_OnCreate(hWnd, wParam, lParam);
		break;

	case WM_DESTROY:
		return MiniMapWnd_OnDestroy(hWnd, wParam, lParam);
		break;

	case WM_CLOSE:
		return MiniMapWnd_OnClose(hWnd, wParam, lParam);
		break;

	case WM_PAINT:
		return MiniMapWnd_OnPaint(hWnd, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
		return MiniMapWnd_OnLButtonDown(hWnd, wParam, lParam);
		break;

	case WM_NCACTIVATE:
		return MiniMapWnd_OnNcActivate(hWnd, wParam, lParam);
		break;

	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT MiniMapWnd_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC	hDCScreen		= NULL;

	HBITMAP	hBitmap			= NULL;

	hDCScreen = ::GetDC(NULL);
	
	MMW_hDCMiniMap = ::CreateCompatibleDC(hDCScreen);

	hBitmap = ::CreateCompatibleBitmap(hDCScreen, 128, 128);
	::SelectObject(MMW_hDCMiniMap, hBitmap);
	::DeleteObject(hBitmap);

	::ReleaseDC(NULL, hDCScreen);

	return 0;
}

LRESULT MiniMapWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT MiniMapWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	::DeleteDC(MMW_hDCMiniMap);

	return ::DefWindowProc(hWnd, WM_DESTROY, wParam, lParam);
}

LRESULT MiniMapWnd_OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HBRUSH	hBrush			= NULL;
	HPEN	hPen			= NULL;
	HBRUSH	hOldBrush		= NULL;
	HPEN	hOldPen			= NULL;

	HDC hdc = ::BeginPaint(hWnd, &ps);

	////////////////////////////////////////////////////////////////////////////////
	// ��С��ͼ
	::BitBlt(hdc, 0, 0, 128, 128, MMW_hDCMiniMap, 0, 0, SRCCOPY);

	////////////////////////////////////////////////////////////////////////////////
	// �����ο�

	// �ջ�ˢ�������������ľ��ξ��ǿ��ĵ�
	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	// ��ɫ��
	hPen = (HPEN)::GetStockObject(WHITE_PEN);

	hOldBrush = (HBRUSH)::SelectObject(hdc, hBrush);
	hOldPen = (HPEN)::SelectObject(hdc, hPen);

	::Rectangle(hdc, MMW_nx, MMW_ny, MMW_nx + 21, MMW_ny + 15);

	::SelectObject(hdc, hOldBrush);
	::SelectObject(hdc, hOldPen);

	::DeleteObject(hBrush);
	::DeleteObject(hPen);

	::EndPaint(hWnd, &ps);

	return ::DefWindowProc(hWnd, WM_PAINT, wParam, lParam);
}

LRESULT MiniMapWnd_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MMW_nx = LOWORD(lParam) - 10;
	MMW_ny = HIWORD(lParam) - 7;

	MMW_nx = MMW_nx < 0 ? 0 : MMW_nx;
	MMW_ny = MMW_ny < 0 ? 0 : MMW_ny;

	// MMW_nx����Ϊż��
	MMW_nx = MMW_nx - MMW_nx % 2;

	if (MapEditWnd_IsActive())
	{
		MapEditWnd_SetCameraPos(MMW_nx, MMW_ny);
	}
	else
	{
		if (TemplateEditWnd_IsActive())
		{
			TemplateEditWnd_SetCameraPos(MMW_nx, MMW_ny);
		}
	}
	// ����ػ�
	::InvalidateRect(hWnd, NULL, FALSE);

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	LRESULT MiniMapWnd_OnNcActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	��ڵı�����ʼ������ɫ��������
// ����ֵ:
//	
// ����  :
//	Wnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT MiniMapWnd_OnNcActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	::DefWindowProc(hWnd, WM_NCACTIVATE, TRUE, 0);
	return 1;
}



VOID MiniMapWnd_Show(int nShow)
{
	::ShowWindow(MMW_hMiniMapWnd, nShow);
}

VOID MiniMapWnd_Move(LONG x, LONG y)
{
	::SetWindowPos(MMW_hMiniMapWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID MiniMapWnd_SetCameraPos(LONG x, LONG y)
//
// ����  :
//	���þ�ͷ���ο��λ��
//
// ����ֵ:
//	
// ����  :
//	x -
//	y -
//
////////////////////////////////////////////////////////////////////////////////
VOID MiniMapWnd_SetCameraPos(LONG x, LONG y)
{
	MMW_nx = (SHORT)x;
	MMW_ny = (SHORT)y;

	::InvalidateRect(MMW_hMiniMapWnd, NULL, FALSE);
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID MiniMapWnd_Paint(VOID)
//
// ����  :
//	������С��ͼ��ͼ��
//
// ����ֵ:
//	
// ����  :
//
////////////////////////////////////////////////////////////////////////////////
VOID MiniMapWnd_Paint(VOID)
{
	MapEx_DrawMiniMap(MMW_hDCMiniMap, (LPWORD)g_MapData, (HDC*)g_hDCMiniTileImage);
	::InvalidateRect(MMW_hMiniMapWnd, NULL, FALSE);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID MiniMapWnd_Paint(LONG x, LONG y)
//
// ����  :
//	����ͼ�ϵ�һ��Tile��ͼ��ʵ���Ͼ���һ������
//
// ����ֵ:
//	
// ����  :
//	x -
//	y -
//
////////////////////////////////////////////////////////////////////////////////
VOID MiniMapWnd_Paint(LONG x, LONG y)
{
	MapEx_UpdateMiniMap(MMW_hDCMiniMap, x, y, (LPWORD)g_MapData, (HDC*)g_hDCMiniTileImage);
	::InvalidateRect(MMW_hMiniMapWnd, NULL, FALSE);
}