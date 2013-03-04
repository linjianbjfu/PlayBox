#ifndef __YLINI_H__
#define __YLINI_H__

#include <windows.h>
#include <tchar.h>


// Number bases
#define BASE_BINARY			2
#define BASE_OCTAL			8
#define BASE_DECIMAL		10
#define BASE_HEXADECIMAL	16
	
class YL_Ini
{

public:		
	YL_Ini(); // Default constructor
	YL_Ini(LPCTSTR lpPathName); // Construct with a given file name
	virtual ~YL_Ini();

	
	void SetPathName(LPCTSTR lpPathName); // Specify a new file name
	
	DWORD GetString(LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR lpBuffer, DWORD dwBufSize, LPCTSTR lpDefault = NULL) const;
	BOOL WriteString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue) const;

	int GetInt(LPCTSTR lpSection, LPCTSTR lpKey, int nDefault, int nBase = BASE_DECIMAL) const;
	BOOL WriteInt(LPCTSTR lpSection, LPCTSTR lpKey, int nValue, int nBase = BASE_DECIMAL) const;
	
	UINT GetUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nDefault, int nBase = BASE_DECIMAL) const;
	BOOL WriteUInt(LPCTSTR lpSection, LPCTSTR lpKey, UINT nValue, int nBase = BASE_DECIMAL) const;
	
	BOOL GetBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bDefault) const;
	BOOL WriteBool(LPCTSTR lpSection, LPCTSTR lpKey, BOOL bValue) const;
	
	double GetDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fDefault) const;
	BOOL WriteDouble(LPCTSTR lpSection, LPCTSTR lpKey, double fValue, int nPrecision = -1) const;

	TCHAR GetChar(LPCTSTR lpSection, LPCTSTR lpKey, TCHAR cDefault) const;
	BOOL WriteChar(LPCTSTR lpSection, LPCTSTR lpKey, TCHAR c) const;

	POINT GetPoint(LPCTSTR lpSection, LPCTSTR lpKey, POINT ptDefault) const;
	BOOL WritePoint(LPCTSTR lpSection, LPCTSTR lpKey, POINT pt) const;
	
	RECT GetRect(LPCTSTR lpSection, LPCTSTR lpKey, RECT rcDefault) const;
	BOOL WriteRect(LPCTSTR lpSection, LPCTSTR lpKey, RECT rc) const;

	DWORD GetDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpBuffer, DWORD dwBufSize, DWORD dwOffset = 0) const;
	BOOL WriteDataBlock(LPCTSTR lpSection, LPCTSTR lpKey, LPCVOID lpData, DWORD dwDataSize) const;
	
	BOOL DeleteSection(LPCTSTR lpSection) const;
	BOOL DeleteValue(LPCTSTR lpSection, LPCTSTR lpKey) const;

	static BOOL StringToBool(LPCTSTR lpString, BOOL bDefault = FALSE);


protected:	

	//------------------------------------------------------------
	//    Helper Functions
	//------------------------------------------------------------
	static LPTSTR __StrDupEx(LPCTSTR lpStart, LPCTSTR lpEnd);
	static BOOL __TrimString(LPTSTR lpBuffer);
	LPTSTR __GetStringDynamic(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault = NULL) const;
	static void __ToBinaryString(UINT nNumber, LPTSTR lpBuffer, DWORD dwBufSize);
	static int __ValidateBase(int nBase);
	static void __IntToString(int nNumber, LPTSTR lpBuffer, int nBase);
	static void __UIntToString(UINT nNumber, LPTSTR lpBuffer, int nBase);

	//------------------------------------------------------------
	//    Member Data
	//------------------------------------------------------------
	LPTSTR m_pszPathName; // Stores path of the associated ini file
};
#endif // #ifndef __INI_H__