////////////////////////////////////////////////////////////////////////////////
//
//                              ��ͼģ��
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		Map.cpp
//
//	˵����
//		������ͼ����
//
//	������
//		
//
//////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#include "Map.h"

// Tile�ڲ�ͬy�����µ�ˮƽ��Ƭ�Ŀ��
LONG g_HSegmentTable[ 32 ] = {
				8,	8,
				16,	16,
				24,	24,
				32,	32,
				40,	40,
				48,	48,
				56,	56,
				64,	64,
				56,	56,
				48,	48,
				40,	40,
				32,	32,
				24,	24,
				16,	16,
				8,	8,
				0,	0
				};

// Tile�ڲ�ͬx�����µĴ�ֱ��Ƭ�ĸ߶�
LONG g_VSegmentTable[ 64 ] = {
				2,	2,	2,	2,
				6,	6,	6,	6,
				10,	10,	10,	10,
				14,	14,	14,	14,
				18,	18,	18,	18,
				22,	22,	22,	22,
				26,	26,	26,	26,
				30,	30,	30,	30,
				30,	30,	30,	30,
				26,	26,	26,	26,
				22,	22,	22,	22,
				18,	18,	18,	18,
				14,	14,	14,	14,
				10,	10,	10,	10,
				6,	6,	6,	6,
				2,	2,	2,	2,
				};

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	inline DWORD GetBit(DWORD dwValue, DWORD dwBit)
//
// ����  :
//	
// ����ֵ:
//	
// ����  :
//	wValue -
//	dwBit -
//
////////////////////////////////////////////////////////////////////////////////
inline DWORD GetBit(DWORD dwValue, DWORD dwBit)
{
	DWORD	Ret	= 0;

	Ret = (dwValue >> (dwBit - 1)) & 1;
	return Ret;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	inline VOID SetBit(DWORD *dwpValue, DWORD dwBit, BOOL bSet)
//
// ����  :
//	
// ����ֵ:
//	
// ����  :
//	dwpValue -
//	dwBit -
//	bSet -
//
////////////////////////////////////////////////////////////////////////////////
inline VOID SetBit(DWORD *dwpValue, DWORD dwBit, BOOL bSet)
{
	if (bSet)
	{
		*dwpValue |= 1 << (dwBit - 1);
	}
	else
	{
		*dwpValue &= ~(1 << (dwBit - 1));
	}
}

VOID Map_SetLayerImage(DWORD *dwpLayer, DWORD dwImage)
{
	// 00010000 11111111
	//    ^     ^^^^^^^^

	DWORD	i	= 0;

	for (i = 1; i <= 8; i++)
	{
		if (GetBit(dwImage, i) == 1)
		{
			SetBit(dwpLayer, i, TRUE);
		}
		else
		{
			SetBit(dwpLayer, i, FALSE);
		}
	}

	if (GetBit(dwImage, 9) == 1)
	{
		SetBit(dwpLayer, 8 + 5, TRUE);
	}
	else
	{
		SetBit(dwpLayer, 8 + 5, FALSE);
	}
}

VOID Map_SetLayerHeight(DWORD *dwpLayer, DWORD dwHeight)
{
	// 00001111 00000000
	//     ^^^^    

	DWORD	i	= 0;

	for (i = 1; i <= 4; i++)
	{
		if (GetBit(dwHeight, i) == 1)
		{
			SetBit(dwpLayer, i + 8, TRUE);
		}
		else
		{
			SetBit(dwpLayer, i + 8, FALSE);
		}
	}
}

VOID Map_SetTileBarrier(DWORD *dwpLayer, BOOL bBarrier)
{
	// 00100000 00000000
	//   ^      

	SetBit(dwpLayer, 8 + 6, bBarrier);
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	DWORD GetLayerImage(DWORD dwLayer) 
//
// ����  :
//	��ȡͼ���ͼ����
//
// ����ֵ:
//	ͼ���ͼ����
//	
// ����  :
//	DWORD dwLayer - ͼ�����ݣ�ʵ������WORD
//
////////////////////////////////////////////////////////////////////////////////
DWORD Map_GetLayerImage(DWORD dwLayer)
{
	// 00010000 11111111
	//    ^     ^^^^^^^^
	DWORD	dwImage	= 0;

	DWORD	i	= 0;

	for (i = 1; i <= 8; i++)
	{
		if (GetBit(dwLayer, i) == 1)
		{
			SetBit(&dwImage, i, TRUE);
		}
	}

	if (GetBit(dwLayer, 8 + 5) == 1)
	{
		SetBit(&dwImage, 9, TRUE);
	}

	return dwImage;
}

DWORD Map_GetLayerHeight(DWORD dwLayer)
{
	// 00001111 00000000
	//     ^^^^    
	DWORD	dwHeight	= 0;

	// ȡ���ֽڵĵ�4λ
	dwHeight = (dwLayer >> 8) & 0xF;

	return dwHeight;
}

BOOL Map_IsBarrier(DWORD dwLayer)
{
	// 00100000 00000000
	//   ^
	if (GetBit(dwLayer, 8 + 6) == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL Map_MapIsExist(DWORD dwMapNumber)
{
	FILE	*fp	= NULL;

	CHAR MapFile[ MAX_PATH ];
	sprintf(MapFile, "Map\\Map%04d", dwMapNumber);

	fp = fopen(MapFile, "rb");

	if (fp == NULL)
	{
		return FALSE;
	}
	else
	{
		fclose(fp);
		return TRUE;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// ������:
//	LONG Map_CaleSegmentPointAtLine(LONG x, LONG a, LONG b)
//
// ����  :
//	��һ��ֱ�����߶�a���߶�b���������ɣ���ֱ���ϵĵ�x�ڵڼ�����
//	�����Ǵ�0����ģ��εı��Ҳ�Ǵ�0����
//
// ����ֵ:
//	��x�ڵڼ�����
//
// ����  :
//	x -
//	a -
//	b -
//
////////////////////////////////////////////////////////////////////////////////
LONG Map_CaleSegmentPointAtLine(LONG x, LONG a, LONG b)
{
	LONG	c		= 0;
	LONG	d		= 0;
	LONG	lSegment	= 0;

	x++;

	c = a + b;

	d = (x - (x % c)) / c;	// ����ǰ���ж��ٶ�a + b

	// ÿ��a + b��������
	lSegment = d * 2;

	// ���������a�Σ�Ҳ���ǵ���b�Σ��ټ�1
	if (x % c > a)
	{
		lSegment++;
	}

	return lSegment;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID Map_CalcXYPixelToTile(LONG lPixelX, LONG lPixelY, LPLONG lpTileX, LPLONG lpTileY)
//
// ����  :
//	�����ؼ���������Tile������
//
// ����ֵ:
//	
// ����  :
//	lMouseX - �����ڵ�ͼ�е�x����
//	lMouseY - �����ڵ�ͼ�е�y����
//	lplTileX - ����Tile X�����ָ��
//	lplTileY - ����Tile Y�����ָ��
//
////////////////////////////////////////////////////////////////////////////////
VOID Map_CalcXYPixelToTile(LONG lPixelX, LONG lPixelY, LPLONG lplTileX, LPLONG lplTileY)
{
	LONG	a		= 0;
	LONG	b		= 0;
	LONG	xt		= 0;
	LONG	yt		= 0;

	// X
	yt = lPixelY % 32;

	a = g_HSegmentTable[ yt ];
	b = 64 - a;

	xt = lPixelX - b / 2;
	*lplTileX = Map_CaleSegmentPointAtLine(xt, a, b);

	// Y
	xt = lPixelX % 64;

	a = g_VSegmentTable[ xt ];
	b = 30 - a;
	yt = lPixelY - b / 2;

	*lplTileY = (yt - yt % 32) / 32;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID Map_CalcXYTileToPixel(LONG lTileX, LONG lTileY, LPLONG lplPixelX, LPLONG lplPixelY)
//
// ����  :
//	����ͼ�����Ͻ��ڵ�ͼ�еĵ����ؼ�����
//
// ����ֵ:
//	
// ����  :
//	lTileX - Tile��Tile��x����
//	lTileY - Tile��Tile��y����
//	lplPixelX - ���ڽ���Tile�����ؼ�x�����ָ��
//	lplPixelY - ���ڽ���Tile�����ؼ�y�����ָ��
//
////////////////////////////////////////////////////////////////////////////////
VOID Map_CalcXYTileToPixel(LONG lTileX, LONG lTileY, LPLONG lplPixelX, LPLONG lplPixelY)
{
	*lplPixelX = lTileX * 32;
	*lplPixelY = lTileY * 32 + (lTileX % 2) * 16;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID Map_CalcXYTileBesideTile(LONG lDirection, LONG lTileX, LONG lTileY, LPLONG lplTileX, LPLONG lplTileY)
//
// ����  :
//	����һ��Tile�ĸ��������ڽ���Tile������
//
//	���·���Ϊ��
//	���·���Ϊ��
//	���Ϸ���Ϊ��
//	���Ϸ���Ϊ��
//
// ����ֵ:
//	
// ����  :
//	lDirection - ����1 - ����2 - �ϣ�3 - ����4 - ��
//	lTileX - ԴTile x����
//	lTileY - ԴTile y����
//	lplTileX - ����Ŀ��Tile x�����ָ��
//	lplTileY - ����Ŀ��Tile y�����ָ��
//
////////////////////////////////////////////////////////////////////////////////
VOID Map_CalcXYTileBesideTile(LONG lDirection, LONG lTileX, LONG lTileY, LPLONG lplTileX, LPLONG lplTileY)
{
	switch (lDirection)
	{
	case 1:// ��
		if (lTileX % 2 == 0)
		{
			*lplTileX = lTileX + 1;
			*lplTileY = lTileY;
		}
		else
		{
			*lplTileX = lTileX + 1;
			*lplTileY = lTileY + 1;
		}
		break;
	case 2:// ��
		if (lTileX % 2 == 0)
		{
			*lplTileX = lTileX - 1;
			*lplTileY = lTileY;
		}
		else
		{
			*lplTileX = lTileX - 1;
			*lplTileY = lTileY + 1;
		}
		break;
	case 3: // ��
		if (lTileX % 2 == 0)
		{
			*lplTileX = lTileX - 1;
			*lplTileY = lTileY - 1;
		}
		else
		{
			*lplTileX = lTileX - 1;
			*lplTileY = lTileY;
		}
		break;
	case 4: // ��
		if (lTileX % 2 == 0)
		{
			*lplTileX = lTileX + 1;
			*lplTileY = lTileY - 1;
		}
		else
		{
			*lplTileX = lTileX + 1;
			*lplTileY = lTileY;
		}
		break;
	}
}

VOID Map_DrawTile(HDC hdcDest, LPDRAWTILESTRUCT lpDT)
{
	LONG	xo		= 0;
	LONG	yo		= 0;
	LONG	xDest		= 0;
	LONG	yDest		= 0;

	LONG	lImage		= 0;
	LONG	lHeight		= 0;

	UINT	uColorKey	= RGB(108, 88, 100);

	Map_CalcXYTileToPixel(lpDT->lxOrigin, lpDT->lyOrigin, &xo, &yo);
	Map_CalcXYTileToPixel(lpDT->lxTile, lpDT->lyTile, &xDest, &yDest);

	// ���������ԭ��Tile���������꣬���ڴ�����ʾ�����ؼ�����
	xDest -= xo;
	yDest -= yo;

	xDest -= 32;
	yDest -= 16;

	// ��һ��
	lImage = Map_GetLayerImage(lpDT->wLayer0Data);
	lHeight = Map_GetLayerHeight(lpDT->wLayer0Data);

	if (lHeight >= lpDT->lHeight0)
	{
		::TransparentBlt(hdcDest, xDest, yDest, 64, 30, lpDT->phDC[ lImage ], 0, 0, 64, 30, uColorKey);
	}

	// �ڶ���
	lImage = Map_GetLayerImage(lpDT->wLayer1Data);
	lHeight = Map_GetLayerHeight(lpDT->wLayer1Data);

	if (lImage > 0)
	{
		if (lHeight >= lpDT->lHeight1)
		{
			::TransparentBlt(hdcDest, xDest, yDest, 64, 30, lpDT->phDC[ lImage - 1 ], 0, 0, 64, 30, uColorKey);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	VOID Map_MarkTile(HDC hdcDest, LPDRAWTILESTRUCT lpDT)
//
// ����  :
//	��Tile�ϴ���
//
// ����ֵ:
//	
// ����  :
//	hdcDest -
//	lpDT -
//
////////////////////////////////////////////////////////////////////////////////
VOID Map_MarkTile(HDC hdcDest, LPDRAWTILESTRUCT lpDT)
{
	LONG	xo		= 0;
	LONG	yo		= 0;
	LONG	xDest		= 0;
	LONG	yDest		= 0;

	Map_CalcXYTileToPixel(lpDT->lxOrigin, lpDT->lyOrigin, &xo, &yo);
	Map_CalcXYTileToPixel(lpDT->lxTile, lpDT->lyTile, &xDest, &yDest);

	// ���������ԭ��Tile���������꣬���ڴ�����ʾ�����ؼ�����
	xDest -= xo;
	yDest -= yo;

	xDest -= 32;
	yDest -= 16;

	::TransparentBlt(hdcDest, xDest, yDest, 64, 30, lpDT->hDCMark, 0, 0, 64, 30, 0);
}

BOOL Map_Assert(LONG x, LONG y)
{
	if (0 <= x && x <= 127 &&
		0 <= y && y <= 127)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	};
}