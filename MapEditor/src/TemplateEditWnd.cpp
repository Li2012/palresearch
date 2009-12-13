////////////////////////////////////////////////////////////////////////////////
//
//                              ģ��༭����ģ��
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		TemplateEditWnd.cpp
//
//	˵����
//
//
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#include "Map.h"
#include "MapEx.h"
#include "ImageSelWnd.h"
#include "Document.h"
#include "ToolBar.h"
#include "Menu.h"
#include "MiniMapWnd.h"
#include "TileAttributeDialog.h"
#include "StatusBar.h"

#include "TemplateEditWnd.h"

// ����WinMain.cpp
extern WORD		g_TemplateData[ 128 ][ 128 ][ 2 ];

extern HDC		g_hDCTileImage[ 512 ];

extern DWORD		g_dwTemplateCount;
extern DRAWTILESTRUCT	g_Template[ MAX_TEMPLATE ];

extern CStatusBar	g_StatusBar;

extern BOOL		g_bAITemplatePaster;

////////////////////////////////////////////////////////////////////////////////
// �ڲ�����
////////////////////////////////////////////////////////////////////////////////

// �����Ƿ��ڼ���״̬
BOOL		TEW_bActive			= FALSE;

// �ӿ����Ͻ�Tile��ģ���ϵ�����
LONG		TEW_lxCamera			= 0;
LONG		TEW_lyCamera			= 0;

// �����ָ��Tile������
LONG		TEW_lxMouseAtTile		= 0;
LONG		TEW_lyMouseAtTile		= 0;

// ��ǰѡ���Tile������
LONG		TEW_lxCurSelTile		= 0;
LONG		TEW_lyCurSelTile		= 0;	

// �༭״̬
// IDCMD_EDIT_SELECT		- ѡ��״̬
// IDCMD_EDIT_PEN		- �༭״̬
// IDCMD_EDIT_TEMPLATE		- ʹ��ģ��
// IDCMD_EDIT_ERASE		- �������
// IDCMD_EDIT_BARRIER		- �����ϰ�
DWORD		TEW_dwStatus			= IDCMD_EDIT_SELECT;

// ��ǰ�༭��ͼ��
DWORD		TEW_dwCurEditLayer			= IDCMD_EDIT_LAYER0;

// �������Ƿ񱻰���
BOOL		TEW_bMouseLBDown			= FALSE;

// �Ƿ���ʾ�������
BOOL		TEW_bShowObject				= FALSE;

// �Ƿ��ʾ�ϰ�Tile
BOOL		TEW_bShowBarrier			= FALSE;

// ͼ��
HDC		TEW_hDCMouseAtTile			= NULL;
HDC		TEW_hDCCurSelTile			= NULL;
HDC		TEW_hDCBarrier				= NULL;
HDC		TEW_hDCObject				= NULL;
HDC		TEW_hDCTemplate				= NULL;

// ��̨��Ļ
HDC		TEW_hDCBack				= NULL;

HWND		TEW_hTemplateEditWnd			= NULL;

const LPCSTR	TEMPLATEEDITWNDCLASS			= "TemplateEditWndClass";
const LPCSTR	TEW_ErrorString				= "����ģ��༭����ʧ�ܣ�";

////////////////////////////////////////////////////////////////////////////////
//
//                       ���ڸ�����Ϣ�Ĵ�����
//
//	TemplateEditWnd_OnCreate
//	TemplateEditWnd_OnDestroy
//	TemplateEditWnd_OnCommand
//	TemplateEditWnd_OnTimer
//
//	TemplateEditWnd_OnMouseMove
//	TemplateEditWnd_OnLButtonDown
//	TemplateEditWnd_OnLButtonUp
//	TemplateEditWnd_OnRButtonDown
//	TemplateEditWnd_OnRButtonUp
//
//	TemplateEditWnd_OnVScroll
//	TemplateEditWnd_OnHScroll
//
//
//	��Щ������TemplateEditWnd_WindowProc����
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK TemplateEditWnd_WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT TemplateEditWnd_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

LRESULT TemplateEditWnd_OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnRButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnRButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

LRESULT TemplateEditWnd_OnVScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT TemplateEditWnd_OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);

LRESULT TemplateEditWnd_OnMDIActivate(HWND hWnd, WPARAM wParam, LPARAM lParam);

VOID TemplateEditWnd_UpdateCommandUI(VOID);

VOID TemplateEditWnd_MarkTemplate(VOID);
////////////////////////////////////////////////////////////////////////////////
//
//				��������
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL TemplateEditWnd_Create(HWND hWndParent)
//
// ����  :
//	����ģ��ര��
//
// ����ֵ:
//	
// ����  :
//	HWND hWndParent - �����ھ��������ΪMDI�ͻ�����
//
////////////////////////////////////////////////////////////////////////////////
BOOL TemplateEditWnd_Create(HWND hWndParent)
{
	MDICREATESTRUCT		MDICreate;
	HINSTANCE		hInstance	= NULL;
	WNDCLASSEX		stWndClass;
	ATOM			atom;

	hInstance = (HINSTANCE)::GetWindowLong(hWndParent, GWL_HINSTANCE);

	////////////////////////////////////////////////////////////////////////////////
	// ע��ģ��༭������
	////////////////////////////////////////////////////////////////////////////////
	stWndClass.cbSize = sizeof(WNDCLASSEX);
	stWndClass.style = CS_HREDRAW | CS_VREDRAW;
	stWndClass.lpfnWndProc = TemplateEditWnd_WindowProc;
	stWndClass.cbClsExtra = 0;
	stWndClass.cbWndExtra = 0;
	stWndClass.hInstance = hInstance;
	stWndClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAPEDIT));
	stWndClass.hCursor = ::LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
	stWndClass.hbrBackground = ::GetSysColorBrush(COLOR_BTNFACE);
	stWndClass.lpszMenuName = NULL;
	stWndClass.lpszClassName = TEMPLATEEDITWNDCLASS;
	stWndClass.hIconSm = NULL;

	atom = RegisterClassEx(&stWndClass);
	if (atom == 0)
	{
		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ����ģ��༭����
	////////////////////////////////////////////////////////////////////////////////
	MDICreate.cx      = 320;
	MDICreate.cy      = 200;
	MDICreate.x       = 0;
	MDICreate.y       = 200;
	MDICreate.hOwner  = hInstance;
	MDICreate.lParam  = 0;
	MDICreate.style   = 0;
	MDICreate.szClass = TEMPLATEEDITWNDCLASS;
	MDICreate.szTitle = "ģ��";

	TEW_hTemplateEditWnd = (HWND)::SendMessage(hWndParent, WM_MDICREATE, 0, (LPARAM)&MDICreate);

	if (TEW_hTemplateEditWnd == NULL)
	{
		::MessageBox(hWndParent, TEW_ErrorString, NULL, MB_OK);
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	LRESULT CALLBACK TemplateEditWnd_WindowProc(
//				       HWND hWnd,
//				       UINT uMsg,
//				       WPARAM wParam,
//				       LPARAM lParam)
//
// ����  :
//	ģ��༭������Ϣ������
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
LRESULT CALLBACK TemplateEditWnd_WindowProc(
				       HWND hWnd,
				       UINT uMsg,
				       WPARAM wParam,
				       LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return TemplateEditWnd_OnCreate(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		return TemplateEditWnd_OnDestroy(hWnd, wParam, lParam);
		break;
	case WM_CLOSE:
		return TemplateEditWnd_OnClose(hWnd, wParam, lParam);
		break;
	case WM_COMMAND:
		return TemplateEditWnd_OnCommand(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		return TemplateEditWnd_OnTimer(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		return TemplateEditWnd_OnMouseMove(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		return TemplateEditWnd_OnLButtonDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		return TemplateEditWnd_OnLButtonUp(hWnd, wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
		return TemplateEditWnd_OnRButtonDown(hWnd, wParam, lParam);
		break;
	case WM_RBUTTONUP:
		return TemplateEditWnd_OnRButtonUp(hWnd, wParam, lParam);
		break;
	case WM_HSCROLL:
		return TemplateEditWnd_OnHScroll(hWnd, wParam, lParam);
		break;
	case WM_VSCROLL:
		return TemplateEditWnd_OnVScroll(hWnd, wParam, lParam);
		break;
	case WM_MDIACTIVATE:
		return TemplateEditWnd_OnMDIActivate(hWnd, wParam, lParam);
		break;
	default:
		return ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_CREATE��Ϣ���ֺ���
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG		lStyleEx		= 0;
	HDC		hDCScreen		= NULL;
	HBRUSH		hBrush			= NULL;
	HBITMAP		hBitmap			= NULL;
	HINSTANCE	hInstance		= NULL;

	// ���ӿͻ��߿���
	lStyleEx = ::GetWindowLong(hWnd, GWL_EXSTYLE);
	lStyleEx |= WS_EX_CLIENTEDGE;
	::SetWindowLong(hWnd, GWL_EXSTYLE, lStyleEx);

	// ���ù�����
	::SetScrollRange(hWnd, SB_VERT, 0, 127, TRUE);	// ��ֱ
	::SetScrollRange(hWnd, SB_HORZ, 0, 63, TRUE);	// ˮƽ

	// ������ɫ�ĺ�̨��Ļ
	hDCScreen	= ::GetDC(NULL);
	TEW_hDCBack = ::CreateCompatibleDC(hDCScreen);

	hBitmap = ::CreateCompatibleBitmap(hDCScreen, 1024, 768);
	::SelectObject(TEW_hDCBack, hBitmap);
	::DeleteObject(hBitmap);

	hBrush = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	::SelectObject(TEW_hDCBack, hBrush);
	::DeleteObject(hBrush);

	// �����ʾ3��Tile��ͼ��
	hInstance = (HINSTANCE)::GetWindowLong(hWnd, GWL_HINSTANCE);

	// ��ɫ����ʾ��ǰ�������Tile
	hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, 0);
	TEW_hDCMouseAtTile = ::CreateCompatibleDC(hDCScreen);
	::SelectObject(TEW_hDCMouseAtTile, hBitmap);
	::DeleteObject(hBitmap);

	// ��ɫ����ʾ��ǰѡ���Tile
	hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, 0);
	TEW_hDCCurSelTile = ::CreateCompatibleDC(hDCScreen);
	::SelectObject(TEW_hDCCurSelTile, hBitmap);
	::DeleteObject(hBitmap);

	// ��ɫ����ʾ�ϰ�Tile
	hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, 0);
	TEW_hDCBarrier = ::CreateCompatibleDC(hDCScreen);
	::SelectObject(TEW_hDCBarrier, hBitmap);
	::DeleteObject(hBitmap);

	// ��ɫ����ʾ��ѡ��ģ���Tile
	hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 0, 0, 0);
	TEW_hDCTemplate = ::CreateCompatibleDC(hDCScreen);
	::SelectObject(TEW_hDCTemplate, hBitmap);
	::DeleteObject(hBitmap);


	hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, 0);
	TEW_hDCObject = ::CreateCompatibleDC(hDCScreen);
	::SelectObject(TEW_hDCObject, hBitmap);
	::DeleteObject(hBitmap);

	::ReleaseDC(NULL, hDCScreen);

	// ���ò˵�����������ťΪ����
	BOOL bEnable = TRUE;
	ToolBar_Enable(IDCMD_FILE_SAVE, bEnable);

	ToolBar_Enable(IDCMD_EDIT_SELECT, bEnable);
	ToolBar_Enable(IDCMD_EDIT_PEN, bEnable);
	ToolBar_Enable(IDCMD_EDIT_TEMPLATE, bEnable);
	ToolBar_Enable(IDCMD_EDIT_ERASE, bEnable);
	ToolBar_Enable(IDCMD_EDIT_BARRIER, bEnable);

	ToolBar_Enable(IDCMD_EDIT_LAYER0, bEnable);
	ToolBar_Enable(IDCMD_EDIT_LAYER1, bEnable);

	ToolBar_Enable(IDCMD_VIEW_BARRIER, bEnable);

	ToolBar_Enable(IDCMD_WINDOW_MAPEDITWND, bEnable);
	ToolBar_Enable(IDCMD_WINDOW_TEMPLATEEDITWND, bEnable);

	Menu_Enable(IDCMD_FILE_SAVE, bEnable);

	Menu_Enable(IDCMD_EDIT_SELECT, bEnable);
	Menu_Enable(IDCMD_EDIT_PEN, bEnable);
	Menu_Enable(IDCMD_EDIT_TEMPLATE, bEnable);
	Menu_Enable(IDCMD_EDIT_ERASE, bEnable);
	Menu_Enable(IDCMD_EDIT_BARRIER, bEnable);

	Menu_Enable(IDCMD_EDIT_LAYER0, bEnable);
	Menu_Enable(IDCMD_EDIT_LAYER1, bEnable);

	Menu_Enable(IDCMD_VIEW_BARRIER, bEnable);
	Menu_Enable(IDCMD_VIEW_OBJECT, bEnable);

	::TemplateEditWnd_UpdateCommandUI();

	TemplateEditWnd_ClearTemplate();
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	LRESULT TemplateEditWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	��ֹ�رմ���
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_DESTROY��Ϣ������
//	���ڹر�ǰ�ͷ�DC
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	::DeleteDC(TEW_hDCMouseAtTile);
	::DeleteDC(TEW_hDCCurSelTile);
	::DeleteDC(TEW_hDCBarrier);
	::DeleteDC(TEW_hDCObject);
	::DeleteDC(TEW_hDCBack);

	return ::DefMDIChildProc(hWnd, WM_DESTROY, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_COMMAND��Ϣ������
//	�ı�༭״̬����ʾ��־�������¹��������˵���״̬
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDCMD_EDIT_LAYER0:
	case IDCMD_EDIT_LAYER1:
		TEW_dwCurEditLayer = LOWORD(wParam);
		break;

	case IDCMD_EDIT_SELECT:
	case IDCMD_EDIT_PEN:
	case IDCMD_EDIT_TEMPLATE:
	case IDCMD_EDIT_ERASE:
		TEW_dwStatus = LOWORD(wParam);
		break;
	case IDCMD_EDIT_BARRIER:
		TEW_dwStatus = LOWORD(wParam);
		TEW_bShowBarrier = TRUE;
		break;
	case IDCMD_VIEW_BARRIER:
		TEW_bShowBarrier = !TEW_bShowBarrier;
		break;
	case IDCMD_VIEW_OBJECT:
		TEW_bShowObject = !TEW_bShowObject;
		break;
	}
	TemplateEditWnd_UpdateCommandUI();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_TIMER��Ϣ������
//	ˢ�´��ڻ���
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	DRAWTILESTRUCT	DrawTile;

	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	POINT	pt;
	::GetCursorPos(&pt);
	if (pt.x == cxScreen - 1)
	{
		::SendMessage(hWnd, WM_HSCROLL, MAKEWPARAM(SB_LINERIGHT, 0), 0);
	}
	if (pt.x == 0)
	{
		::SendMessage(hWnd, WM_HSCROLL, MAKEWPARAM(SB_LINELEFT, 0), 0);
	}
	if (pt.y == 0)
	{
		::SendMessage(hWnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
	}
	if (pt.y == cyScreen - 1)
	{
		::SendMessage(hWnd, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
	}

	// ����
	::Rectangle(TEW_hDCBack, 0, 0, 1028, 768);

	MapEx_Draw(TEW_hDCBack, 45, 30, (LPWORD)g_TemplateData, (HDC*)g_hDCTileImage, TEW_lxCamera, TEW_lyCamera);

	if (TEW_bShowObject)
	{
		::MapEx_ViewObject(
			TEW_hDCBack,
			TEW_lxCurSelTile,
			TEW_lyCurSelTile,
			TEW_hDCObject,
			(LPWORD)g_TemplateData,
			g_hDCTileImage,
			TEW_lxCamera,
			TEW_lyCamera
			);
	}

	if (TEW_dwStatus == IDCMD_EDIT_TEMPLATE)
	{
		MapEx_DrawTemplate(TEW_hDCBack, (LPWORD)g_TemplateData, TEW_lxMouseAtTile, TEW_lyMouseAtTile, g_Template, g_dwTemplateCount, TEW_lxCamera, TEW_lyCamera, g_bAITemplatePaster);
	}

	if (TEW_bShowBarrier)
	{
		MapEx_MarkBarrier(TEW_hDCBack, 45, 30, (LPWORD)g_TemplateData, TEW_hDCBarrier, TEW_lxCamera, TEW_lyCamera);
	}

	memset(&DrawTile, 0, sizeof(DRAWTILESTRUCT));

	DrawTile.lxOrigin = TEW_lxCamera;
	DrawTile.lyOrigin = TEW_lyCamera;
	DrawTile.lxTile = TEW_lxMouseAtTile;
	DrawTile.lyTile = TEW_lyMouseAtTile;
	DrawTile.hDCMark = TEW_hDCMouseAtTile;

	Map_MarkTile(TEW_hDCBack, &DrawTile);

	memset(&DrawTile, 0, sizeof(DRAWTILESTRUCT));

	DrawTile.lxOrigin = TEW_lxCamera;
	DrawTile.lyOrigin = TEW_lyCamera;
	DrawTile.lxTile = TEW_lxCurSelTile;
	DrawTile.lyTile = TEW_lyCurSelTile;
	DrawTile.hDCMark = TEW_hDCCurSelTile;

	Map_MarkTile(TEW_hDCBack, &DrawTile);

	TemplateEditWnd_MarkTemplate();
//	MapEx_DrawMiniMap(TEW_hDCBack, (LPWORD)g_TemplateData, g_hDCMiniTileImage);

	hDC = ::GetDC(hWnd);

	::BitBlt(hDC, 0, 0, 1024, 768, TEW_hDCBack, 0, 0, SRCCOPY);

	::ReleaseDC(hWnd, hDC);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_MOUSEMOVE��Ϣ������
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG	lCurSelImage	= -1;
	LONG	lLayer		= 0;

	LONG	xt		= 0;
	LONG	yt		= 0;

	// ����������������굱ǰ��ָ��Tile
	Map_CalcXYTileToPixel(TEW_lxCamera, TEW_lyCamera, &xt, &yt);
	
	xt += LOWORD(lParam) + 32;
	yt += HIWORD(lParam) + 16;

	Map_CalcXYPixelToTile(xt, yt, &TEW_lxMouseAtTile, &TEW_lyMouseAtTile);

	g_StatusBar.SetXY(TEW_lxMouseAtTile, TEW_lyMouseAtTile);

	if (TEW_dwCurEditLayer == IDCMD_EDIT_LAYER0)
	{
		lLayer = 0;
	}
	else
	{
		if (TEW_dwCurEditLayer == IDCMD_EDIT_LAYER1)
		{
			lLayer = 1;
		}
	}

	if (!Map_Assert(TEW_lxMouseAtTile, TEW_lyMouseAtTile))
	{
		return 0;
	}

	if (TEW_bMouseLBDown)
	{
		switch (TEW_dwStatus)
		{
		case IDCMD_EDIT_PEN:
			lCurSelImage = ImageSelWnd_GetSelImage();
			if (lCurSelImage < 0)
			{
				return 0;
			}
			Map_SetLayerImage((DWORD*)&g_TemplateData[ TEW_lyMouseAtTile ][ TEW_lxMouseAtTile ][ lLayer ], lCurSelImage + lLayer);
			MiniMapWnd_Paint(TEW_lxMouseAtTile, TEW_lyMouseAtTile);
			Document_SetEditFlag(TRUE);
			break;

		case IDCMD_EDIT_ERASE:
			g_TemplateData[ TEW_lyMouseAtTile ][ TEW_lxMouseAtTile ][ lLayer ] = 0;
			Document_SetEditFlag(TRUE);
			break;

		case IDCMD_EDIT_BARRIER:
			Map_SetTileBarrier((DWORD*)&g_TemplateData[ TEW_lyMouseAtTile ][ TEW_lxMouseAtTile ][ 0 ], !Map_IsBarrier(g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ]));
			Document_SetEditFlag(TRUE);
			break;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_LBUTTONDOWN��Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG	lCurSelImage	= -1;
	LONG	lLayer		= 0;

	LONG	xt		= 0;
	LONG	yt		= 0;

	// ������굱ǰ��ָ��Tile
	Map_CalcXYTileToPixel(TEW_lxCamera, TEW_lyCamera, &xt, &yt);
	
	xt += LOWORD(lParam) + 32;
	yt += HIWORD(lParam) + 16;

	Map_CalcXYPixelToTile(xt, yt, &TEW_lxMouseAtTile, &TEW_lyMouseAtTile);

	TEW_lxCurSelTile = TEW_lxMouseAtTile;
	TEW_lyCurSelTile = TEW_lyMouseAtTile;

	if (TEW_dwCurEditLayer == IDCMD_EDIT_LAYER0)
	{
		lLayer = 0;
	}
	else
	{
		if (TEW_dwCurEditLayer == IDCMD_EDIT_LAYER1)
		{
			lLayer = 1;
		}
	}

	if (!Map_Assert(TEW_lxCurSelTile, TEW_lyCurSelTile))
	{
		return 0;
	}

	if (wParam == (MK_CONTROL | MK_LBUTTON))
	{
		if (g_dwTemplateCount < MAX_TEMPLATE)
		{
			// ����������
			g_Template[ g_dwTemplateCount ].lxTile = TEW_lxCurSelTile;
			g_Template[ g_dwTemplateCount ].lyTile = TEW_lyCurSelTile;
			g_Template[ g_dwTemplateCount ].wLayer0Data = g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ];
			g_Template[ g_dwTemplateCount ].wLayer1Data = g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 1 ];
			g_Template[ g_dwTemplateCount ].hDCMark = TEW_hDCTemplate;
			g_Template[ g_dwTemplateCount ].phDC = g_hDCTileImage;
			g_dwTemplateCount++;
		}
		
		return 0;
	}

	switch (TEW_dwStatus)
	{
	case IDCMD_EDIT_PEN:
		lCurSelImage = ImageSelWnd_GetSelImage();
		if (lCurSelImage < 0)
		{
			return 0;
		}
		TEW_bMouseLBDown = TRUE;
		::SetCapture(hWnd);
		Map_SetLayerImage((DWORD*)&g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ lLayer ], lCurSelImage + lLayer);
		MiniMapWnd_Paint(TEW_lxCurSelTile, TEW_lyCurSelTile);
		Document_SetEditFlag(TRUE);
		break;

	case IDCMD_EDIT_ERASE:
		TEW_bMouseLBDown = TRUE;
		::SetCapture(hWnd);
		g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ lLayer ] = 0;
		Document_SetEditFlag(TRUE);
		break;

	case IDCMD_EDIT_BARRIER:
		TEW_bMouseLBDown = TRUE;
		::SetCapture(hWnd);
		Map_SetTileBarrier((DWORD*)&g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ], !Map_IsBarrier(g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ]));
		Document_SetEditFlag(TRUE);
		break;

	case IDCMD_EDIT_TEMPLATE:
		MapEx_PasterTemplate((LPWORD)g_TemplateData, TEW_lxCurSelTile, TEW_lyCurSelTile, g_Template, g_dwTemplateCount, g_bAITemplatePaster);
		Document_SetEditFlag(TRUE);
		break;
	}

	TileAttributeDialog_Update(&g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ], &g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 1 ]);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_LBUTTONUP��Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (TEW_bMouseLBDown == TRUE)
	{
		TEW_bMouseLBDown = FALSE;
		::ReleaseCapture();
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnRButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_RBUTTONUP��Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnRButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG	xt		= 0;
	LONG	yt		= 0;

	// ����������������굱ǰ��ָ��Tile
	Map_CalcXYTileToPixel(TEW_lxCamera, TEW_lyCamera, &xt, &yt);
	
	xt += LOWORD(lParam) + 32;
	yt += HIWORD(lParam) + 16;

	Map_CalcXYPixelToTile(
		xt,
		yt,
		&TEW_lxMouseAtTile,
		&TEW_lyMouseAtTile);

	TEW_lxCurSelTile = TEW_lxMouseAtTile;
	TEW_lyCurSelTile = TEW_lyMouseAtTile;

	TileAttributeDialog_Update(&g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ], &g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 1 ]);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnRButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_RBUTTONUP��Ϣ������
//	��ʱû��
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnRButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	// �һ��˵�
/*	HINSTANCE hInstance;
	HMENU hPopupMenu, g_hMenu;
	UINT uFlags;
	POINT Point;

	Point.x = 0;
	Point.y = 0;
	::ClientToScreen(hWnd, &Point);

	uFlags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_LEFTBUTTON;
	hInstance = (HINSTANCE)::GetWindowLong(g_hWnd, GWL_HINSTANCE);
	g_hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_POPUP));
	hPopupMenu = GetSubMenu(g_hMenu, 0); 
   	::TrackPopupMenu(hPopupMenu, uFlags, Point.x + LOWORD(lParam), Point.y + HIWORD(lParam), 0, g_hWnd, NULL);
	::DestroyMenu(g_hMenu);
	*/

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnVScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_VSCROLL��Ϣ������
//	��ֱ��������Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnVScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int nPos;
	switch (LOWORD(wParam))
	{
	case SB_LINEUP:
		nPos = ::GetScrollPos(hWnd, SB_VERT);
		if (nPos > 0)
		{
			nPos--;
			TEW_lyCamera = nPos;
			::SetScrollPos(hWnd, SB_VERT, nPos, TRUE);
		}
		break;
	case SB_LINEDOWN:
		nPos = ::GetScrollPos(hWnd, SB_VERT);
		if (nPos < 127)
		{
			nPos++;
			TEW_lyCamera = nPos;
			::SetScrollPos(hWnd, SB_VERT, nPos, TRUE);
		}
		break;
	case SB_THUMBTRACK:
		nPos = HIWORD(wParam);
		TEW_lyCamera = nPos;
		::SetScrollPos(hWnd, SB_VERT, HIWORD(wParam), TRUE);
		break;
	case SB_PAGEUP:
		nPos = ::GetScrollPos(hWnd, SB_VERT);
		nPos--;
		TEW_lyCamera = nPos;
		::SetScrollPos(hWnd, SB_VERT, nPos, TRUE);
		break;
	case SB_PAGEDOWN:
		nPos = ::GetScrollPos(hWnd, SB_VERT);
		nPos++;
		::SetScrollPos(hWnd, SB_VERT, nPos, TRUE);
		TEW_lyCamera = nPos;
		break;
	}

	MiniMapWnd_SetCameraPos(TEW_lxCamera, TEW_lyCamera);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
//
// ����  :
//	WM_HSCROLL��Ϣ������
//	ˮƽ��������Ϣ������
//
// ����ֵ:
//	
// ����  :
//	hWnd -
//	wParam -
//	lParam -
//
////////////////////////////////////////////////////////////////////////////////
LRESULT TemplateEditWnd_OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int nPos;
	switch (LOWORD(wParam))
	{
	case SB_LINELEFT:
		nPos = ::GetScrollPos(hWnd, SB_HORZ);
		if (nPos > 0)
		{
			nPos--;
			TEW_lxCamera = nPos * 2;
			::SetScrollPos(hWnd, SB_HORZ, nPos, TRUE);
		}
		break;
	case SB_LINERIGHT:
		nPos = ::GetScrollPos(hWnd, SB_HORZ);
		if (nPos < 63)
		{
			nPos++;
			::SetScrollPos(hWnd, SB_HORZ, nPos, TRUE);
			TEW_lxCamera = nPos * 2;
		}
		break;
	case SB_THUMBTRACK:
		nPos = HIWORD(wParam);
		::SetScrollPos(hWnd, SB_HORZ, HIWORD(wParam), TRUE);
		TEW_lxCamera = nPos * 2;
		break;
	case SB_PAGELEFT:
		nPos = ::GetScrollPos(hWnd, SB_HORZ);
		nPos--;
		::SetScrollPos(hWnd, SB_HORZ, nPos, TRUE);
		TEW_lxCamera = nPos * 2;
		break;
	case SB_PAGERIGHT:
		nPos = ::GetScrollPos(hWnd, SB_HORZ);
		nPos++;
		::SetScrollPos(hWnd, SB_HORZ, nPos, TRUE);
		TEW_lxCamera = nPos * 2;
		break;
	}

	MiniMapWnd_SetCameraPos(TEW_lxCamera, TEW_lyCamera);
	return 0;
}

LRESULT TemplateEditWnd_OnMDIActivate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == hWnd)
	{
		::SetTimer(hWnd, 0, 40, NULL);
		TEW_bActive = TRUE;

		MiniMapWnd_SetCameraPos(TEW_lxCamera, TEW_lyCamera);
		TileAttributeDialog_Update(&g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 0 ], &g_TemplateData[ TEW_lyCurSelTile ][ TEW_lxCurSelTile ][ 1 ]);
		TemplateEditWnd_UpdateCommandUI();
	}
	else
	{
		::KillTimer(hWnd, 0);
		TEW_bActive = FALSE;
	}

	return ::DefMDIChildProc(hWnd, WM_MDIACTIVATE, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_SetCameraPos(LONG x, LONG y)
//
// ����  :
//	���ñ༭���ڵ��ӿ�λ��
//
// ����ֵ:
//	
// ����  :
//	x -
//	y -
//
////////////////////////////////////////////////////////////////////////////////
VOID TemplateEditWnd_SetCameraPos(LONG x, LONG y)
{
	// x ��0 - 126��ż����Ҫ����2���ǹ������ķ�Χ
	if (Map_Assert(x / 2, y))
	{
		::SendMessage(TEW_hTemplateEditWnd, WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, x / 2), 0);
		::SendMessage(TEW_hTemplateEditWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, y), 0);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID TemplateEditWnd_UpdateCommandUI(HWND hWnd)
//
// ����  :
//	����������棨���������˵�����̬״
//
// ����ֵ:
//	
// ����  :
//	HWND hWnd -
//
////////////////////////////////////////////////////////////////////////////////
VOID TemplateEditWnd_UpdateCommandUI(VOID)
{
	ToolBar_Check(IDCMD_EDIT_SELECT, FALSE);
	ToolBar_Check(IDCMD_EDIT_PEN, FALSE);
	ToolBar_Check(IDCMD_EDIT_TEMPLATE, FALSE);
	ToolBar_Check(IDCMD_EDIT_ERASE, FALSE);
	ToolBar_Check(IDCMD_EDIT_BARRIER, FALSE);

	ToolBar_Check(IDCMD_EDIT_LAYER0, FALSE);
	ToolBar_Check(IDCMD_EDIT_LAYER1, FALSE);

	ToolBar_Check(IDCMD_VIEW_BARRIER, FALSE);

	Menu_Check(IDCMD_EDIT_SELECT, FALSE);
	Menu_Check(IDCMD_EDIT_PEN, FALSE);
	Menu_Check(IDCMD_EDIT_TEMPLATE, FALSE);
	Menu_Check(IDCMD_EDIT_ERASE, FALSE);
	Menu_Check(IDCMD_EDIT_BARRIER, FALSE);

	Menu_Check(IDCMD_EDIT_LAYER0, FALSE);
	Menu_Check(IDCMD_EDIT_LAYER1, FALSE);

	Menu_Check(IDCMD_VIEW_BARRIER, FALSE);

	Menu_Check(IDCMD_VIEW_OBJECT, FALSE);


	ToolBar_Check(TEW_dwStatus, TRUE);
	Menu_Check(TEW_dwStatus, TRUE);

	ToolBar_Check(TEW_dwCurEditLayer, TRUE);
	Menu_Check(TEW_dwCurEditLayer, TRUE);

	if (TEW_bShowBarrier)
	{
		ToolBar_Check(IDCMD_VIEW_BARRIER, TRUE);
		Menu_Check(IDCMD_VIEW_BARRIER, TRUE);
	}

	if (TEW_bShowObject)
	{
		Menu_Check(IDCMD_VIEW_OBJECT, TRUE);
	}
}

VOID TemplateEditWnd_Show(int nShow)
{
	::ShowWindow(TEW_hTemplateEditWnd, nShow);
}

VOID TemplateEditWnd_SetActive(VOID)
{
	::SetActiveWindow(TEW_hTemplateEditWnd);
}

VOID TemplateEditWnd_SetTitle(LPCSTR lpstrTitle)
{
	if (lpstrTitle != NULL)
	{
		::SetWindowText(TEW_hTemplateEditWnd, lpstrTitle);
	}
}

VOID TemplateEditWnd_MarkTemplate(VOID)
{
	DWORD	i	= 0;

	for (i = 0; i < g_dwTemplateCount; i++)
	{
		g_Template[ i ].lxOrigin = TEW_lxCamera;
		g_Template[ i ].lyOrigin = TEW_lyCamera;
		Map_MarkTile(TEW_hDCBack, &g_Template[ i ]);
	}
}

VOID TemplateEditWnd_ClearTemplate(VOID)
{
	memset(&g_Template, 0, sizeof(DRAWTILESTRUCT) * MAX_TEMPLATE);
	g_dwTemplateCount = 0;
}

BOOL TemplateEditWnd_IsActive(VOID)
{
	return TEW_bActive;
}