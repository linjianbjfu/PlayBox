#include "YL_Ini.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define DEF_PROFILE_NUM_LEN		64 // numeric string length, could be quite long for binary format
#define DEF_PROFILE_THRESHOLD	512 // temporary string length
#define DEF_PROFILE_DELIMITER	_T(",") // default string delimiter
#define DEF_PROFILE_TESTSTRING	_T("{63788286-AE30-4D6B-95DF-3B451C1C79F9}") // Uuid 

struct STR_LIMIT
{
	LPTSTR lpTarget;
	DWORD dwRemain;
	DWORD dwTotalCopied;
};
/////////////////////////////////////////////////////////////////////////////////
// Constructors & Destructor
/////////////////////////////////////////////////////////////////////////////////
YL_Ini::YL_Ini()
{
	m_pszPathName = NULL;
}

YL_Ini::YL_Ini(LPCTSTR lpPathName)
{
	m_pszPathName = NULL;
	SetPathName(lpPathName);
}

YL_Ini::~YL_Ini()
{
	if (m_pszPathName != NULL)
	{
	//因为m_pszPathName是用_strdup生成，而_strdup调用的malloc来分配内存，因此需要用free来释放空间
		free(m_pszPathName);
		m_pszPathName = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Ini File Path Access
/////////////////////////////////////////////////////////////////////////////////

// Assign ini file path name
void YL_Ini::SetPathName(LPCTSTR lpPathName)
{
	if (lpPathName == NULL)
	{
		if (m_pszPathName != NULL)
			*m_pszPathName = _T('\0');
	}
	else
	{
		if (m_pszPathName != NULL)
		{
			free(m_pszPathName);
			m_pszPathName = NULL;
		}
		m_pszPathName = _tcsdup(lpPathName);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Raw String Access
/////////////////////////////////////////////////////////////////////////////////

// Get a profile string value, if the buffer size is not large enough, the result
// may be truncated.
DWORD YL_Ini::GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDefault) const
{
	if (lpBuffer != NULL)
		*lpBuffer = _T('\0');

	LPTSTR psz = __GetStringDynamic(lpSection, lpKey, lpDefault);
	DWORD dwLen = (DWORD)_tcslen(psz);

	if (lpBuffer != NULL)
	{
		_tcsncpy(lpBuffer, psz, dwBufSize);
		dwLen = min(dwLen, dwBufSize);
	}

	delete [] psz;
	return dwLen;
}

// Write a string value to the ini file
BOOL YL_Ini::WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue) const
{
	if (lpSection == NULL || lpKey == NULL)
		return FALSE;

	return ::WritePrivateProfileString(lpSection, lpKey, lpValue == NULL ? _T("") : lpValue, m_pszPathName);
}

/////////////////////////////////////////////////////////////////////////////////
// Primitive Data Type Access
/////////////////////////////////////////////////////////////////////////////////

// Get a signed integral value
int YL_Ini::GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault, int nBase) const
{
	TCHAR sz[DEF_PROFILE_NUM_LEN + 1] = _T("");
	GetString(lpSection, lpKey, sz, DEF_PROFILE_NUM_LEN);
	return *sz == _T('\0') ? nDefault : int(_tcstoul(sz, NULL, __ValidateBase(nBase)));
}

// Get an unsigned integral value
UINT YL_Ini::GetUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nDefault, int nBase) const
{
	TCHAR sz[DEF_PROFILE_NUM_LEN + 1] = _T("");
	GetString(lpSection, lpKey, sz, DEF_PROFILE_NUM_LEN);
	return *sz == _T('\0') ? nDefault : UINT(_tcstoul(sz, NULL, __ValidateBase(nBase)));
}

// Get a boolean value
BOOL YL_Ini::GetBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault) const
{
	TCHAR sz[DEF_PROFILE_NUM_LEN + 1] = _T("");
	GetString(lpSection, lpKey, sz, DEF_PROFILE_NUM_LEN);
	return StringToBool(sz, bDefault);
}

// Get a double floating value
double YL_Ini::GetDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fDefault) const
{
	TCHAR sz[DEF_PROFILE_NUM_LEN + 1] = _T("");
	GetString(lpSection, lpKey, sz, DEF_PROFILE_NUM_LEN);
	return *sz == _T('\0') ? fDefault : _tcstod(sz, NULL);
}

// Write a signed integral value to the ini file
BOOL YL_Ini::WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue, int nBase) const
{
	TCHAR szValue[DEF_PROFILE_NUM_LEN + 1] = _T("");
	__IntToString(nValue, szValue, nBase);
	return WriteString(lpSection, lpKey, szValue);
}

// Write an unsigned value to the ini file
BOOL YL_Ini::WriteUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nValue, int nBase) const
{
	TCHAR szValue[DEF_PROFILE_NUM_LEN + 1] = _T("");
	__UIntToString(nValue, szValue, nBase);
	return WriteString(lpSection, lpKey, szValue);
}

// Write a double floating value to the ini file
BOOL YL_Ini::WriteDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fValue, int nPrecision) const
{
	TCHAR szFmt[16] = _T("%f");

	if (nPrecision > 0)
		_sntprintf(szFmt, sizeof(szFmt)-1, _T("%%.%df"), nPrecision);

	TCHAR szValue[DEF_PROFILE_NUM_LEN + 1] = _T("");
	_sntprintf(szValue, DEF_PROFILE_NUM_LEN, szFmt, fValue);
	return WriteString(lpSection, lpKey, szValue);
}


// Write a boolean value to the ini file
BOOL YL_Ini::WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue) const
{
	return WriteInt(lpSection, lpKey, bValue ? 1 : 0, BASE_DECIMAL);
}

TCHAR YL_Ini::GetChar(LPCTSTR lpSection, LPCTSTR lpKey, TCHAR cDefault) const
{
	TCHAR sz[2] = _T("");
	GetString(lpSection, lpKey, sz, 1);
	return *sz == _T('\0') ? cDefault : sz[0];
}

BOOL YL_Ini::WriteChar(LPCTSTR lpSection, LPCTSTR lpKey, TCHAR c) const
{
	TCHAR sz[2] = { c, _T('\0') };
	return WriteString(lpSection, lpKey, sz);
}

/////////////////////////////////////////////////////////////////////////////////
// User-Defined Data Type Access
/////////////////////////////////////////////////////////////////////////////////

// Get a block of raw data from the ini file
DWORD YL_Ini::GetDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpBuffer, DWORD dwBufSize, DWORD dwOffset) const
{
	LPTSTR psz = __GetStringDynamic(lpSection, lpKey);
	DWORD dwLen = (DWORD)_tcslen(psz) / 2;
	if (dwLen <= dwOffset)
	{
		delete [] psz;
		return 0;
	}

	// verify psz, must be all in hex format
	for (int i = 0; psz[i] != _T('\0'); i++)
	{
		TCHAR c = psz[i];
		if ((c >= _T('0') && c <= _T('9'))
			|| (c >= _T('a') && c <= _T('f'))
			|| (c >= _T('A') && c <= _T('F')))
		{
			// valid
		}
		else
		{
			delete [] psz;
			return 0;
		}
	}

	DWORD dwProcLen = 0;
	LPBYTE lpb = (LPBYTE)lpBuffer;

	if (lpb != NULL)
	{
		dwProcLen = min(dwLen - dwOffset, dwBufSize);
		LPCTSTR p = &psz[dwOffset * 2];
		for (DWORD i = 0; i < dwProcLen; i++)
		{			
			TCHAR sz[3] = _T("");
			_tcsncpy(sz, p, 2);			
			lpb[i] = BYTE(_tcstoul(sz, NULL, 16));
			p = &p[2];
		}			
	}
	else
	{
		dwProcLen = dwLen - dwOffset;
	}
	delete [] psz;
	return dwProcLen;
}

// Write a block of raw data to the ini file
BOOL YL_Ini::WriteDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPCVOID lpData, DWORD dwDataSize) const
{
	const BYTE* lpb = (const BYTE*)lpData;
	if (lpb == NULL)
		return FALSE;

	LPTSTR psz = new TCHAR[dwDataSize * 2 + 1];
	for (DWORD i = 0, j = 0; i < dwDataSize; i++, j += 2)
		_stprintf(&psz[j], _T("%02X"), lpb[i]);
	const BOOL RES = WriteString(lpSection, lpKey, psz);
	delete [] psz;
	return RES;
}

// Get a POINT value
POINT YL_Ini::GetPoint(LPCTSTR lpSection, LPCTSTR lpKey, POINT ptDefault) const
{
	POINT pt;
	if (GetDataBlock(lpSection, lpKey, &pt, sizeof(POINT)) != sizeof(POINT))
		pt = ptDefault;
	return pt;
}

// Get a RECT value
RECT YL_Ini::GetRect(LPCTSTR lpSection, LPCTSTR lpKey, RECT rcDefault) const
{
	RECT rc;
	if (GetDataBlock(lpSection, lpKey, &rc, sizeof(RECT)) != sizeof(RECT))
		rc = rcDefault;
	return rc;
}

// Write a POINT to the ini file
BOOL YL_Ini::WritePoint(LPCTSTR lpSection, LPCTSTR lpKey, POINT pt) const
{
	return WriteDataBlock(lpSection, lpKey, &pt, sizeof(POINT));
}

// Write a RECT to the ini file
BOOL YL_Ini::WriteRect(LPCTSTR lpSection, LPCTSTR lpKey, RECT rc) const
{
	return WriteDataBlock(lpSection, lpKey, &rc, sizeof(RECT));
}

// Remove whole section from the ini file
BOOL YL_Ini::DeleteSection(LPCTSTR lpSection) const
{
	return ::WritePrivateProfileString(lpSection, NULL, _T(""), m_pszPathName);
}

// Remove a key from a section
BOOL YL_Ini::DeleteValue(LPCTSTR lpSection, LPCTSTR lpKey) const
{
	return ::WritePrivateProfileString(lpSection, lpKey, NULL, m_pszPathName);
}

/////////////////////////////////////////////////////////////////////////////////
// Helper Functions
/////////////////////////////////////////////////////////////////////////////////

// Get a profile string value, return a heap pointer so we do not have to worry
// about the buffer size, however, this function requires the caller to manually
// free the memory.
// This function is the back-bone of all "Getxxx" functions of this class.
LPTSTR YL_Ini::__GetStringDynamic(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault) const
{
	TCHAR* psz = NULL;
	if (lpSection == NULL || lpKey == NULL)
	{
		// Invalid section or key name, just return the default string
		if (lpDefault == NULL)
		{
			// Empty string
			psz = new TCHAR[1];
			*psz = _T('\0');
		}
		else
		{
			psz = new TCHAR[_tcslen(lpDefault) + 1];
			_tcscpy(psz, lpDefault);
		}
		
		return psz;
	}
	
	// Keep enlarging the buffer size until being certain on that the string we
	// retrieved was original(not truncated).
	DWORD dwLen = DEF_PROFILE_THRESHOLD;
	psz = new TCHAR[dwLen + 1];
	DWORD dwCopied = ::GetPrivateProfileString(lpSection, lpKey, lpDefault == NULL ? _T("") : lpDefault, psz, dwLen, m_pszPathName);
	while (dwCopied + 1 >= dwLen)
	{		
		dwLen += DEF_PROFILE_THRESHOLD;
		delete [] psz;
		psz = new TCHAR[dwLen + 1];
		dwCopied = ::GetPrivateProfileString(lpSection, lpKey, lpDefault == NULL ? _T("") : lpDefault, psz, dwLen, m_pszPathName);
	}
	
	return psz; // !!! Requires the caller to free this memory !!!
}


// Convert an integer into binary string format
void YL_Ini::__ToBinaryString(UINT nNumber, LPTSTR lpBuffer, DWORD dwBufSize)
{
	if (dwBufSize == 0)
		return;
	
	DWORD dwIndex = 0;	
	do
	{
		lpBuffer[dwIndex++] = (nNumber % 2) ? _T('1') : _T('0');
		nNumber /= 2;
	} while (nNumber > 0 && dwIndex < dwBufSize);

	lpBuffer[dwIndex] = _T('\0');
	_tcsrev(lpBuffer);
}

// Make sure the base will be expected value
int YL_Ini::__ValidateBase(int nBase)
{
	switch (nBase)
	{
	case BASE_BINARY:
	case BASE_OCTAL:
	case BASE_HEXADECIMAL:
		break;

	default:
		nBase = BASE_DECIMAL;
	}

	return nBase;
}

// Convert a signed integer into string representation, based on its base
void YL_Ini::__IntToString(int nNumber, LPTSTR lpBuffer, int nBase)
{
	switch (nBase)
	{
	case BASE_BINARY:
	case BASE_OCTAL:
	case BASE_HEXADECIMAL:
		__UIntToString((UINT)nNumber, lpBuffer, nBase);
		break;

	default:
		_stprintf(lpBuffer, _T("%d"), nNumber);
		break;
	}	
}

// Convert an unsigned integer into string representation, based on its base
void YL_Ini::__UIntToString(UINT nNumber, LPTSTR lpBuffer, int nBase)
{
	switch (nBase)
	{
	case BASE_BINARY:
		__ToBinaryString(nNumber, lpBuffer, DEF_PROFILE_NUM_LEN);
		break;

	case BASE_OCTAL:
		_stprintf(lpBuffer, _T("%o"), nNumber);
		break;

	case BASE_HEXADECIMAL:
		_stprintf(lpBuffer, _T("%X"), nNumber);
		break;

	default:
		_stprintf(lpBuffer, _T("%u"), nNumber);
		break;
	}	
}

BOOL YL_Ini::StringToBool(LPCTSTR lpString, BOOL bDefault)
{
	// Default: empty string
	// TRUE: "true", "yes", non-zero decimal numner
	// FALSE: all other cases
	if (lpString == NULL || *lpString == _T('\0'))
		return bDefault;

	return (_tcsicmp(lpString, _T("true")) == 0
		|| _tcsicmp(lpString, _T("yes")) == 0
		|| _tcstol(lpString, NULL, BASE_DECIMAL) != 0);
}

BOOL YL_Ini::__TrimString(LPTSTR lpString)
{
	if (lpString == NULL)
		return FALSE;

	BOOL bTrimmed = FALSE;
	int nLen = (int)_tcslen(lpString);

	// '\n' and '\r' are actually not possible in this case, but anyway...
	
	// Trim right side
	while (nLen >= 0
		&& (lpString[nLen - 1] == _T(' ')
			|| lpString[nLen - 1] == _T('\t')
			|| lpString[nLen - 1] == _T('\r')
			|| lpString[nLen - 1] == _T('\n')))
	{
		lpString[--nLen] = _T('\0');
		bTrimmed = TRUE;		
	}

	// Trim left side
	LPCTSTR p = lpString; 
	while (*p == _T(' ')
			|| *p == _T('\t')
			|| *p == _T('\r')
			|| *p == _T('\n'))
	{
		p = &p[1];
		bTrimmed = TRUE;
	}

	if (p != lpString)
	{
		LPTSTR psz = _tcsdup(p);
		_tcscpy(lpString, psz);
		delete [] psz;
	}

	return bTrimmed;
}

LPTSTR YL_Ini::__StrDupEx(LPCTSTR lpStart, LPCTSTR lpEnd)
{
	const DWORD LEN = ((DWORD)(lpEnd - lpStart)) / sizeof(TCHAR);
	LPTSTR psz = new TCHAR[LEN + 1];
	_tcsncpy(psz, lpStart, LEN);
	psz[LEN] = _T('\0');
	return psz; // !!! Requires the caller to free this memory !!!
}

/////////////////////////////////////////////////////////////////////////////////
// End of YL_Ini Class Implementation
/////////////////////////////////////////////////////////////////////////////////

// If you are getting this error:
// ----------------------------------------------------------------------------
// "fatal error C1010: unexpected end of file while looking for precompiled
//  header directive"
//-----------------------------------------------------------------------------
// Please scroll all the way up and uncomment '#include "stdafx.h"'
