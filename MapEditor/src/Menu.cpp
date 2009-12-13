#include <windows.h>
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////
// �ڲ�����
////////////////////////////////////////////////////////////////////////////////

HMENU		M_hMenu	= NULL;

// ������ʾ
const	LPCSTR	M_ErrorString = "�����˵�ʧ�ܣ�";

BOOL Menu_Create(HWND hWndParent)
{
	HINSTANCE hInstance = NULL;

	hInstance = (HINSTANCE)::GetWindowLong(hWndParent, GWL_HINSTANCE);

	if (hInstance == NULL)
	{
		::MessageBox(hWndParent, M_ErrorString, NULL, MB_OK);
		return FALSE;
	}

	M_hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU));

	if (M_hMenu == NULL)
	{
		::MessageBox(hWndParent, M_ErrorString, NULL, MB_OK);
		return FALSE;
	}

	return ::SetMenu(hWndParent, M_hMenu);
}

VOID Menu_Enable(UINT uID, BOOL bEnable)
{
	UINT uEnable = bEnable ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED;
	::EnableMenuItem(M_hMenu, uID, uEnable);
}

VOID Menu_Check(UINT uID, BOOL bChecked)
{
	UINT uChecked = bChecked ? MF_BYCOMMAND | MF_CHECKED : MF_BYCOMMAND | MF_UNCHECKED;
	::CheckMenuItem(M_hMenu, uID, uChecked);
}