////////////////////////////////////////////////////////////////////////////////
//
//                              ������
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		ToolBar.cpp
//
//	˵����
//		����������ťҪ�������εģ���ɫΪ192,192,192
//
//	������
//		
//
//////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#include "ToolBar.h"

////////////////////////////////////////////////////////////////////////////////
// �ڲ�����
////////////////////////////////////////////////////////////////////////////////

HWND	TB_hWndToolBar		= NULL;

const LPCSTR	TB_ErrorString1 = "����������ʧ�ܣ�";


////////////////////////////////////////////////////////////////////////////////
//
//                              ��������
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SubClassToolBarProc(
				     HWND hWnd,
				     UINT uMsg,
				     WPARAM wParam,
				     LPARAM lParam
				     );

BOOL ToolBar_SetToolBarBitmap(HBITMAP hBitmap);
UINT ToolBar_LoadButtonID(UINT uIDResource, UINT *lpIDArray, UINT uArrayLen);
BOOL ToolBar_SetButtons(const UINT* lpIDArray, INT nIDCount);

VOID ToolBar_OnSizeParent(HWND hWndParent, RECT *pRect);



////////////////////////////////////////////////////////////////////////////////
//
//
//                              ��������
//
//
////////////////////////////////////////////////////////////////////////////////

BOOL ToolBar_Create(HWND hWndParent)
{
	HINSTANCE	hInstance	= NULL;
	HBITMAP		hBitmap		= NULL;
	DWORD		dwButtonCount	= 0;

	UINT		uID[ 128 ];
	::memset(uID, 0, sizeof(UINT) * 128);

	hInstance = (HINSTANCE)::GetWindowLong(hWndParent, GWL_HINSTANCE);

	TB_hWndToolBar = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		TOOLBARCLASSNAME,
		NULL,
		WS_CHILD |
		WS_VISIBLE |
		WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS |
		TBSTYLE_TRANSPARENT |
		TBSTYLE_FLAT |
		TBSTYLE_TOOLTIPS,
		0,
		0,
		0,
		0,
		hWndParent,
		(HMENU)IDR_TOOLBAR,
		hInstance,
		NULL
		);

	if (TB_hWndToolBar == NULL)
	{
		::MessageBox(hWndParent, TB_ErrorString1, NULL, MB_OK);
		return FALSE;
	}

	::SendMessage(TB_hWndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
 
	// ���ù�������λͼ
	hBitmap = (HBITMAP)::LoadBitmap(hInstance, MAKEINTRESOURCE(IDR_TOOLBAR));
	ToolBar_SetToolBarBitmap(hBitmap);

	// ���밴ť
	dwButtonCount = ToolBar_LoadButtonID(IDR_TOOLBAR, uID, 128);
	ToolBar_SetButtons(uID, dwButtonCount);

	ToolBar_Enable(IDCMD_FILE_OPEN, TRUE);

	::ShowWindow(TB_hWndToolBar, SW_SHOW);
	::UpdateWindow(TB_hWndToolBar);
	return TRUE;
}

HWND ToolBar_GetHandle(VOID)
{
	return TB_hWndToolBar;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL ToolBar_SetToolBarBitmap(HBITMAP hBitmap)
//
// ����  :
//	���ù������İ�ťͼ��
//
// ����ֵ:
//	
// ����  :
//	HBITMAP hBitmap - λͼ���
//
////////////////////////////////////////////////////////////////////////////////
BOOL ToolBar_SetToolBarBitmap(HBITMAP hBitmap)
{
	BITMAP Bitmap;

	::GetObject(hBitmap, sizeof(BITMAP), &Bitmap);

	HIMAGELIST hImageList = ImageList_Create(Bitmap.bmHeight, Bitmap.bmHeight, ILC_COLOR16 | ILC_MASK, 0 , 64);

	ImageList_AddMasked(hImageList, hBitmap, RGB(192, 192, 192));

	::SendMessage(TB_hWndToolBar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	UINT ToolBar_LoadButtonID(UINT uIDResource, UINT *lpIDArray, UINT uArrayLen)
//
// ����  :
//	���빤������ťID
//
// ����ֵ:
//	���ذ�ť�ĸ���
//
// ����  :
//	IDResource	- ��ԴID
//	*lpIDArray	- ���հ�ťID�Ļ�����ָ��
//	uArrayLen	- ���հ�ťID�Ļ������Ĵ�С
//
////////////////////////////////////////////////////////////////////////////////
UINT ToolBar_LoadButtonID(UINT uIDResource, UINT *lpIDArray, UINT uArrayLen)
{
	// ��ȡ��������Դ��Ϣ����
	HRSRC hRsrc = ::FindResource(NULL, MAKEINTRESOURCE(uIDResource), MAKEINTRESOURCE(241));
	if (hRsrc == NULL)
	{
		return 0;
	}

	// ��ȡ��Դ���
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
	if (hGlobal == NULL)
	{
		return 0;
	}

	// ��ȡ��Դ����ָ��
	CToolBarData *pData = (CToolBarData*)::LockResource(hGlobal);
	if (pData == NULL)
	{
		return 0;
	}

	if (pData->wItemCount > uArrayLen)
	{
		return 0;
	}

	// ��ȡ��ťID
	for (int i = 0; i < pData->wItemCount; i++)
	{
		lpIDArray[ i ] = pData->items()[i];
	}

	return pData->wItemCount;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL ToolBar_SetButtons(const UINT* lpIDArray, INT nIDCount)
//
// ����  :
//	���ù�������ť
//
// ����ֵ:
//	
// ����  :
//	pIDArray - ��ťID����ָ��
//	nIDCount - ��ť����
//
////////////////////////////////////////////////////////////////////////////////
BOOL ToolBar_SetButtons(const UINT* lpIDArray, INT nIDCount)
{
	TBBUTTON Button;
	memset(&Button, 0, sizeof(TBBUTTON));
	Button.iString = -1;
	if (lpIDArray != NULL)
	{
		// Add new Buttons to the toolbar.
		int iImage = 0;
		for (int i = 0; i < nIDCount; i++)
		{
			if ((Button.idCommand = *lpIDArray++) == 0)
			{
				// separator
				Button.fsStyle = TBSTYLE_SEP;
				Button.iBitmap = 8;
			}
			else
			{
				// A command Button with image.
				Button.fsStyle = TBSTYLE_BUTTON;
				Button.iBitmap = iImage++;
			}
			if (!::SendMessage(
				TB_hWndToolBar,
				TB_ADDBUTTONS,
				1,
				(LPARAM)&Button)
				)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

VOID ToolBar_Enable(UINT uButtonID, BOOL bEnable)
{
	::SendMessage(TB_hWndToolBar, TB_ENABLEBUTTON, (WPARAM)uButtonID, (LPARAM)MAKELPARAM(bEnable, 0));
}

VOID ToolBar_Check(UINT uButtonID, BOOL bCheck)
{
	::SendMessage(TB_hWndToolBar, TB_CHECKBUTTON, (WPARAM)uButtonID, MAKELPARAM(bCheck, 0));
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID ToolBar_SizeParent(RECT *lpRect)
//
// ����  :
//	�������ڵĳߴ緢���仯ʱ�������������������¸������ڵĲ���
//	
// ����ֵ:
//	
// ����  :
//	RECT *lpRect - �����ڵĿͻ����ߴ�
//
////////////////////////////////////////////////////////////////////////////////
VOID ToolBar_SizeParent(RECT *lpRect)
{
	if (lpRect == NULL)
	{
		return;
	}

	LONG	lHeight		= 0;
	RECT	rc;

	// ��Ȼ����0�������������Լ������óߴ�
	::MoveWindow(TB_hWndToolBar, 0, 0, 0, 0, TRUE);

	::GetWindowRect(TB_hWndToolBar, &rc);
	lHeight = rc.bottom - rc.top;

	lpRect->top += lHeight;
}