/********************************************************************************\
|
|	Name:
|		RLE.h
|
|	Description:
|		RLE��ش���
|
\********************************************************************************/

BOOL DecodeRLE(LPBYTE lpImageCode, LPBYTE lpBuffer, DWORD dwBufferLen);

DWORD GetWidth(LPBYTE lpImageCode);

DWORD GetHeight(LPBYTE lpImageCode);