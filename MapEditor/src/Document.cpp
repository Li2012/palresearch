////////////////////////////////////////////////////////////////////////////////
//
//                              �ĵ�
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		Document.cpp
//
//	˵����
//		
//
//	������
//		
//
//////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>

#include "ImageGroup.h"
#include "FileDialog.h"
#include "Bitmap.h"

#include "Document.h"

// ��ͼ����
extern WORD		g_MapData[ 128 ][ 128 ][ 2 ];

// ģ������
extern WORD		g_TemplateData[ 128 ][ 128 ][ 2 ];

// ͼ��ͼ��
extern DWORD		g_dwImageCount;
extern HDC		g_hDCTileImage[ 512 ];
extern HDC		g_hDCMiniTileImage[ 512 ];

// ��ɫ��
extern RGBQUAD		g_Palette[ 256 ];

// �ļ���
CHAR			DOC_FileName[ MAX_PATH ];

// �ļ�ָ��
FILE			*DOC_fpMap	= NULL;

// ��ͼ�Ƿ��޸��ˣ���ͼ�������ΪFALSE
BOOL			DOC_bEdit	= FALSE;

const DWORD	MAP_LEN			= 1024 * 64;

////////////////////////////////////////////////////////////////////////////////
//
//                              ��������
//
////////////////////////////////////////////////////////////////////////////////
BOOL Document_LoadImage(LPSTR lpFilePath);


////////////////////////////////////////////////////////////////////////////////
//
//                              ��������
//
////////////////////////////////////////////////////////////////////////////////

VOID Document_Init(VOID)
{
	HBITMAP		hBitmap		= NULL;
	HDC		hDCScreen	= NULL;
	LONG		i		= 0;

	hDCScreen = ::GetDC(NULL);

	for (i = 0; i < 512; i++)
	{
		g_hDCTileImage[ i ] = ::CreateCompatibleDC(hDCScreen);

		g_hDCMiniTileImage[ i ] = ::CreateCompatibleDC(hDCScreen);

		hBitmap = (HBITMAP)::CreateCompatibleBitmap(hDCScreen, 1, 1);
		::SelectObject(g_hDCMiniTileImage[ i ], hBitmap);
		::DeleteObject(hBitmap);
	}

	::ReleaseDC(NULL, hDCScreen);
}

VOID Document_Release(VOID)
{
	LONG		i		= 0;

	for (i = 0; i < 512; i++)
	{
		::DeleteObject(g_hDCTileImage[ i ]);
		::DeleteObject(g_hDCMiniTileImage[ i ]);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID Document_OpenFile(HWND hWndParent)
//
// ����  :
//	������ͼ���ݡ�������ͼͼ��λͼ
//	
// ����ֵ:
//	
// ����  :
//	HWND hWndParent - �ĵ��Ի���ĸ�����
//
////////////////////////////////////////////////////////////////////////////////
BOOL Document_OpenFile(HWND hWndParent)
{
	CHAR	Buffer[ MAX_PATH ];

	DWORD	dwLen		= 0;

	if (!Document_CloseFile(hWndParent))
	{
		return FALSE;
	}

	if (OpenFileDialog_DoModal(hWndParent))
	{
		FileDialog_GetPathName(Buffer);
		FileDialog_GetFileName(DOC_FileName);

		DOC_fpMap = fopen(Buffer, "rb+");
		if (DOC_fpMap == NULL)
		{
			::sprintf(Buffer, "���ļ�%s��ʧ�ܣ�", Buffer);
			::MessageBox(hWndParent, Buffer, NULL, MB_OK);
			return FALSE;
		}

		// ����ͼ����
		fread(g_MapData, MAP_LEN, 1, DOC_fpMap);

		// ��ģ������
		fread(g_TemplateData, MAP_LEN, 1, DOC_fpMap);

		// ��ͼ��
		dwLen = ::strlen(Buffer);
		::sprintf(Buffer, "Gop\\Gop%s", Buffer + dwLen - 4);
		Document_LoadImage(Buffer);

		Document_SetEditFlag(FALSE);

		return TRUE;
	}
	return FALSE;
}

BOOL Document_SaveFile(VOID)
{
	if (DOC_fpMap == NULL)
	{
		return FALSE;
	}

	fseek(DOC_fpMap, 0, SEEK_SET);
	fwrite(g_MapData, MAP_LEN, 1, DOC_fpMap);
	fwrite(g_TemplateData, MAP_LEN, 1, DOC_fpMap);

	Document_SetEditFlag(FALSE);

	return TRUE;
}

BOOL Document_CloseFile(HWND hWndParent)
{
	INT	nRet	= 0;

	if (DOC_fpMap != NULL)
	{
		if (DOC_bEdit)
		{
			nRet = ::MessageBox(hWndParent, "��ͼ���ݱ��Ķ��ˡ�\nҪ�����ļ���", "�ر��ļ�", MB_YESNOCANCEL | MB_ICONEXCLAMATION);
			switch (nRet)
			{
			case IDYES:
				if (!Document_SaveFile())
				{
					::MessageBox(hWndParent, "�����ļ�ʧ�ܡ�", NULL, MB_OK);
					return FALSE;
				}
				break;
			case IDNO:
				break;
			case IDCANCEL:
				return FALSE;
				break;
			}
		}

		fclose(DOC_fpMap);
		DOC_fpMap = NULL;
	}
	return TRUE;
}

VOID Document_NewFile()
{
/*	FILE	*fpSrcMap	= NULL;
	DWORD	dwLen		= 0;

	CHAR	SrcMapFile[ MAX_PATH ];
	CHAR	NewMapFile[ MAX_PATH ];
	CHAR	MapName[ MAX_PATH ];

	::memset(SrcMapFile, 0, MAX_PATH);
	::memset(NewMapFile, 0, MAX_PATH);
	::memset(MapName, 0, MAX_PATH);

	if (lpstrName == NULL)
	{
		return FALSE;
	}

	if (dwNewMapNumber == dwSrcMapNumber)
	{
		return FALSE;
	}

	sprintf(SrcMapFile, "Map\\Map%d", dwSrcMapNumber);
	sprintf(NewMapFile, "Map\\Map%d", dwNewMapNumber);

	if (!::CopyFile(SrcMapFile, NewMapFile, FALSE))
	{
		return FALSE;
	}

	fpNewMap = fopen(NewMapFile, "rb+");
	if (fpNewMap == NULL)
	{
		return FALSE;
	}

	strcpy(MapName, lpstrName);
	fwrite(MapName, MAX_PATH, 1, fpNewMap);

	::memset(Map, 0, 1024 * 64);
	fwrite(Map, 1024 * 64, 1, fpNewMap);

	fclose(fpNewMap);*/
}

BOOL Document_LoadImage(LPSTR lpFilePath)
{
	static BYTE	ImageCode[ 1024 * 256 ];// ���ͼ��ѹ������
	static BYTE	TileImage[ 32 * 15 ];// ���ͼ������

	DWORD	dwImageCount	= 0;
	DWORD	dwLen		= 0;

	HBITMAP	hBitmap1x	= NULL;
	HBITMAP	hBitmap2x	= NULL;
	HBITMAP	hBitmapOld	= NULL;

	DWORD	i		= 0;
	FILE	*fpGop		= NULL;

	if (lpFilePath == NULL)
	{
		return FALSE;
	}

	fpGop = fopen(lpFilePath, "rb");

	if (fpGop == NULL)
	{
		return FALSE;
	}

	// ͼ�����ݵĳ���
	fread(&dwLen, sizeof(DWORD), 1, fpGop);

	fread(ImageCode, dwLen, 1, fpGop);

	// ����ͼ�飬�Ŵ�1��
	g_dwImageCount = ((WORD*)ImageCode)[ 0 ] - 1;
	for (i = 0; i < g_dwImageCount; i++)
	{
		GetImage(ImageCode, i, TileImage, 32 * 15);

		hBitmap1x = CreateBitmapFrom8Bits(TileImage, 32, 15, g_Palette);

		hBitmap2x = ZoomIn(hBitmap1x, 2);

		hBitmapOld = (HBITMAP)::SelectObject(g_hDCTileImage[ i ], hBitmap2x);

		::DeleteObject(hBitmap1x);
		::DeleteObject(hBitmapOld);

		// �����ͼ�õ�
		::BitBlt(g_hDCMiniTileImage[ i ], 0, 0, 1, 1, g_hDCTileImage[ i ], 32, 15, SRCCOPY);
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID Document_SetEditFlag(BOOL bEdit)
//
// ����  :
//	�ڵ�ͼ���ڻ�ģ�崰�����޸�������Ҫ���������������ΪTRUE
//	�ļ��������Ҫ���������������ΪFALSE
//
// ����ֵ:
//	
// ����  :
//	BOOL bEdit -
//
////////////////////////////////////////////////////////////////////////////////
VOID Document_SetEditFlag(BOOL bEdit)
{
	DOC_bEdit = bEdit;
}


LPSTR Document_GetFileName(VOID)
{
	return DOC_FileName;
}