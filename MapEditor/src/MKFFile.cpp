////////////////////////////////////////////////////////////////////////////////
//
//                            MKF�ļ���ش���
//
////////////////////////////////////////////////////////////////////////////////
//
//	�ļ�����
//		MKFFile.cpp
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

#include "MKFFile.h"

// ������ʾ
const	LPCSTR	MKF_ErrorString1	= "��ȡMKF��¼�ų��硣";
const	LPCSTR	MKF_ErrorString2	= "��ȡMKF��¼���������㡣";

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	DWORD MKF_GetRecordCount(FILE *fp)
//
// ����  :
//	��ȡmkf�ļ��еļ�¼����
//	
// ����ֵ:
//	mkf�ļ��еļ�¼����
//	
// ����  :
//	FILE *fp - mkf�ļ�ָ��
//
////////////////////////////////////////////////////////////////////////////////
DWORD MKF_GetRecordCount(FILE *fp)
{
	DWORD dwRecordCount	=	0;
	if (fp == NULL)
	{
		return 0;
	}

	fseek(fp, 0, SEEK_SET);
	fread(&dwRecordCount, sizeof(DWORD), 1, fp);

	dwRecordCount = (dwRecordCount - 4) / 4;

	return dwRecordCount;
}

////////////////////////////////////////////////////////////////////////////////
//
// ������:
//	LONG MKF_ReadRecord(LPBYTE lpBuffer, DWORD dwBufferLength, DWORD dwRecord, FILE *fp)
//
// ����  :
//	���ļ��ж�ȡһ����¼������lpBuffer��
//
// ����ֵ:
//	���ش��ڵ���0����ʾ��¼�ĳ���
//	����-1����ʾ�����д���
//	����-2����ʾ���������㡣
//	
// ����  :
//	lpBuffer -		������ָ��
//	dwBufferLength -	�������ֽڵ�λ����
//	dwRecord -		�����¼��
//	*fp -			�ļ�ָ��
//
////////////////////////////////////////////////////////////////////////////////
LONG MKF_ReadRecord(LPBYTE lpBuffer, DWORD dwBufferLength, DWORD dwRecord, FILE *fp)
{
	DWORD dwOffset       = 0;
	DWORD dwNextOffset   = 0;
	DWORD dwRecordCount  = 0;
	DWORD dwRecordLen    = 0;

	if (lpBuffer == NULL ||	fp == NULL || dwBufferLength == 0)
	{
		return -1;
	}

	// �����¼��
	dwRecordCount = MKF_GetRecordCount(fp);

	if (dwRecord >= dwRecordCount)
	{
		::MessageBox(::GetActiveWindow(), MKF_ErrorString1, NULL, MB_OK);
		return -1;
	}

	// ��ȡ��¼�ĵ�ַ
	fseek(fp, 4 * dwRecord, SEEK_SET);
	fread(&dwOffset, 4, 1, fp);
	fread(&dwNextOffset, 4, 1, fp);

	// ��¼����
	dwRecordLen = dwNextOffset - dwOffset;

	if (dwRecordLen > dwBufferLength)
	{
		::MessageBox(::GetActiveWindow(), MKF_ErrorString2, NULL, MB_OK);
		return -2;
	}

	if (dwRecordLen != 0)
	{
		memset(lpBuffer, 0, dwBufferLength);
		fseek(fp, dwOffset, SEEK_SET);
		fread(lpBuffer, dwRecordLen, 1, fp);
	}
	return dwRecordLen;
}