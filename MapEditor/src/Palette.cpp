#include <windows.h>
#include <stdio.h>
#include "MKFFile.h"


const	LPCSTR	P_ErrorString	= "�򿪵�ɫ���ļ�Pat.mkfʧ�ܣ�\n��ȷ���ͱ�������ͬĿ¼����Pat.mkf�ļ���";



////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	BOOL Palette_Init(RGBQUAD *pPalette)
//
// ����  :
//	��ȡ�ɽ��ĵ�ɫ�岢ת�����ʺ�window�ĸ�ʽ
//
// ����ֵ:
//	
// ����  :
//	RGBQUAD *pPalette - ���յ�ɫ�����ݵĻ�������ָ��
//
////////////////////////////////////////////////////////////////////////////////
BOOL Palette_Init(RGBQUAD *pPalette)
{
	LONG	lResult	= 0;
	FILE	*fp	= NULL;

	// �ɽ��ĵ�ɫ������
	static	BYTE	PaletteCode[ 3 * 512 ];

	if (pPalette == NULL)
	{
		return FALSE;
	}

	fp = fopen("Pat.mkf", "rb");
	if (fp == NULL)
	{
		::MessageBox(::GetActiveWindow(), P_ErrorString, NULL, MB_OK);
		return FALSE;
	}

	lResult = MKF_ReadRecord(PaletteCode, 3 * 512, 0, fp);
	fclose(fp);

	if (lResult <= 0)
	{
		return FALSE;
	}

	DWORD	i	= 0;
	FLOAT	rate	= 0.2f;
	for (i = 0; i < 255; i++)
	{
		// ����Ԫɫ��0 -- 63������0 - 255
		pPalette[ i ].rgbRed = PaletteCode[ i * 3 + 0] * 4;
		pPalette[ i ].rgbGreen = PaletteCode[ i * 3 + 1] * 4;
		pPalette[ i ].rgbBlue = PaletteCode[ i * 3 + 2] * 4;
		pPalette[ i ].rgbReserved = 0;

		// ���һ�㱥�Ͷ�(�ɽ���ͼ��Ƚ���ɫ)
		pPalette[ i ].rgbRed += (BYTE)((255 - pPalette[ i ].rgbRed) * rate);
		pPalette[ i ].rgbGreen += (BYTE)((255 - pPalette[ i ].rgbGreen) * rate);
		pPalette[ i ].rgbBlue += (BYTE)((255 - pPalette[ i ].rgbBlue) * rate);
	}

	// �ؼ�ɫ����
	pPalette[ i ].rgbRed = PaletteCode[ i * 3 + 0] * 4;
	pPalette[ i ].rgbGreen = PaletteCode[ i * 3 + 1] * 4;
	pPalette[ i ].rgbBlue = PaletteCode[ i * 3 + 2] * 4;
	pPalette[ i ].rgbReserved = 0;

	return TRUE;
}