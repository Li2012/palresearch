/********************************************************************************\
|                           
|
|	Name:
|		Bitmap.h
|
|	Description:
|		Bitmap����
|
\********************************************************************************/

HBITMAP	CreateBitmapFrom8Bits(BYTE *pData, DWORD dwWidth, DWORD dwHeight, RGBQUAD *pPalette);
HBITMAP ZoomIn(HBITMAP hBitmap, DWORD dwN);
