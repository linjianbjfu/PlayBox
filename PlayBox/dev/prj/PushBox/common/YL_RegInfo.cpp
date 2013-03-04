#include "YL_RegInfo.h"

bool YL_RegInfo::CreateKey( HKEY hRegKey, const string &strRegEntry )
{
	HKEY hKey; 		
	LONG err;
	err = RegCreateKeyEx(hRegKey, strRegEntry.c_str(), 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,&hKey, NULL );

	if( err == ERROR_SUCCESS )
	{
		if( RegCloseKey( hKey ) == ERROR_SUCCESS )
		{
			return true;
		}
	}
		
	return false;		
}

bool YL_RegInfo::CreateKey( HKEY hRegKey, const char* pszRegEntry )
{
	if( pszRegEntry == NULL )
	{
		return false;
	}
	return CreateKey( hRegKey,string(pszRegEntry) );
}

bool YL_RegInfo::DeleteKey( HKEY hRegKey, const string &strRegEntry, const string& strKeyName)
{		
	if( strKeyName == "" )
	{
		LONG lResult;
		DWORD dwSize;
		TCHAR szName[MAX_PATH];
		HKEY hKey;
		FILETIME ftWrite;
		
		string strSubKey = strRegEntry;
				

		// First, see if we can delete the key without having
		// to recurse.

		lResult = RegDeleteKey(hRegKey, strSubKey.c_str());

		if (lResult == ERROR_SUCCESS) 
			return true;

		lResult = RegOpenKeyEx (hRegKey, strSubKey.c_str(), 0, KEY_READ, &hKey);

		if (lResult != ERROR_SUCCESS) 
		{
			if (lResult == ERROR_FILE_NOT_FOUND) 
			{
				return true;
			} 
			else 
			{			
				return false;
			}
		}

		// Check for an ending slash and add one if it is missing.

		if( strSubKey[strSubKey.length()-1] != '\\' )
		{
			strSubKey += "\\";
		}

		// Enumerate the keys

		dwSize = MAX_PATH;
		lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
			NULL, NULL, &ftWrite);

		if (lResult == ERROR_SUCCESS) 
		{
			do {
				
				string strTmp = strSubKey;
				strTmp += szName;

				if (!DeleteKey(hRegKey, strTmp.c_str()))
				{
					break;
				}

				dwSize = MAX_PATH;

				lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
					NULL, NULL, &ftWrite);

			} while (lResult == ERROR_SUCCESS);
		}

		RegCloseKey (hKey);

		// Try again to delete the key.

		lResult = RegDeleteKey(hRegKey, strRegEntry.c_str());

		if (lResult == ERROR_SUCCESS) 
			return true;

		return false;
	} 
	else
	{
		HKEY hKey; 
		LONG err ; 
		err = RegOpenKeyEx(hRegKey, strRegEntry.c_str(),0, KEY_ALL_ACCESS, &hKey);
		if(err == ERROR_SUCCESS) 
		{ 
			err = RegDeleteValue(hKey, strKeyName.c_str()); 
			if( (err == ERROR_SUCCESS && RegCloseKey(hKey) == ERROR_SUCCESS ) || (err == ERROR_FILE_NOT_FOUND ) )
			{
				return true;
			}
			RegCloseKey( hKey );
		} 
		else if( err == ERROR_FILE_NOT_FOUND )
		{
			return true;
		}

		return false;
	}
}

bool YL_RegInfo::DeleteKey( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName)
{
	if( pszRegEntry == NULL )
	{
		return false;
	}
	if( pszKeyName == NULL )
	{
		return DeleteKey( hRegKey,string(pszRegEntry) );
	}
	else
	{
		return DeleteKey( hRegKey,string(pszRegEntry),string(pszKeyName) );
	}
}

LONG YL_RegInfo::DeleteValue(HKEY regClass, const string &regEntry, const string &valueName)
{
	HKEY hKey; 
	LONG err ; 
	err = RegOpenKeyEx(regClass, regEntry.c_str(),0, KEY_ALL_ACCESS, &hKey);
	if(err == ERROR_SUCCESS) 
	{ 
		err = RegDeleteValue(hKey, valueName.c_str()); 
		RegCloseKey(hKey); 
	} 

	return err;
}

LONG YL_RegInfo::DeleteSubKey(HKEY regClass, const string &regEntry, const string &keyName)
{
	HKEY hKey; 
	LONG err ; 
	err = RegOpenKeyEx(regClass, regEntry.c_str(),0, KEY_ALL_ACCESS, &hKey);
	if(err == ERROR_SUCCESS) 
	{ 
		err = RegDeleteKey(hKey, keyName.c_str()); 
		RegCloseKey(hKey); 
	} 

	return err;
}

bool YL_RegInfo::WriteString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , const string& strKeyValue)
{
	HKEY hKey;
	DWORD dw = (DWORD)strKeyValue.size(); 
	LONG err = -1; 
	DWORD regType = REG_SZ; 	

	err = RegOpenKeyEx( hRegKey, strRegEntry.c_str(),0, KEY_SET_VALUE, &hKey ) ;
	if(err == ERROR_SUCCESS)
	{ 
		err = RegSetValueEx( hKey, strKeyName.c_str() , 0, regType, (LPBYTE) strKeyValue.c_str(), dw ); 
		if( err == ERROR_SUCCESS && RegCloseKey(hKey) == ERROR_SUCCESS )
		{
			return true;
		}
		RegCloseKey( hKey );
	} 
	else if( err == ERROR_FILE_NOT_FOUND )
	{		
		if( CreateKey( hRegKey,strRegEntry) )
		{
			HKEY hKey;
			DWORD dw = (DWORD)strKeyValue.size(); 
			LONG err = -1; 
			DWORD regType = REG_SZ; 	

			err = RegOpenKeyEx( hRegKey, strRegEntry.c_str(),0, KEY_SET_VALUE, &hKey ) ;
			if(err == ERROR_SUCCESS)
			{ 
				err = RegSetValueEx( hKey, strKeyName.c_str() , 0, regType, (LPBYTE) strKeyValue.c_str(), dw ); 
				if( err == ERROR_SUCCESS && RegCloseKey(hKey) == ERROR_SUCCESS )
				{
					return true;
				}
				RegCloseKey( hKey );
			} 			
		}
	}

	return false;
}

bool YL_RegInfo::WriteString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const char* pszKeyValue)
{
	if( pszRegEntry == NULL || pszKeyName == NULL || pszKeyValue == NULL )
	{
		return false;
	}
	return WriteString( hRegKey,string(pszRegEntry),string(pszKeyName),string(pszKeyValue) );
}


bool YL_RegInfo::ReadString(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , string& strKeyValue)
{
	HKEY hKey; 
	DWORD dw = 0; 
	LONG err ; 
	DWORD regType = REG_SZ;
	char buf1[1];
	
	buf1[0] = 0;
	err = RegOpenKeyEx( hRegKey, strRegEntry.c_str() ,0, KEY_READ, &hKey );
	if(err == ERROR_SUCCESS) 
	{
		err = RegQueryValueEx( hKey, strKeyName.c_str() , 0, &regType,(LPBYTE) buf1, &dw); 
		if(err == ERROR_MORE_DATA)
		{
			char* buf2 = new char[dw + 1];
			err = RegQueryValueEx(hKey, strKeyName.c_str() , 0, &regType,(LPBYTE) buf2 , &dw); 
			buf2[dw] = 0;
			strKeyValue = buf2;
			delete[] buf2;
			if( RegCloseKey(hKey) == ERROR_SUCCESS )
			{
				return true;
			}
		}	
		RegCloseKey( hKey );
	} 

	return false;
}

bool YL_RegInfo::ReadString( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,char szBuffer[],unsigned int unBuffLen)
{
	if( pszRegEntry == NULL || pszKeyName == NULL)
	{
		return false;
	}
	string strKeyValue;
	if( ReadString( hRegKey,string(pszRegEntry),string(pszKeyName),strKeyValue) && strKeyValue.length() <= unBuffLen - 1  )
	{
		strcpy( szBuffer,strKeyValue.c_str() );
		return true;
	}
	return false;
}


bool YL_RegInfo::WriteDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName ,const unsigned int& unValue)
{

	HKEY hKey;
	DWORD dw = 4; 
	LONG err = -1; 
	DWORD regType = REG_DWORD; 	

	err = RegOpenKeyEx(hRegKey, strRegEntry.c_str(),0, KEY_SET_VALUE, &hKey) ;
	if(err == ERROR_SUCCESS)
	{ 
		err = RegSetValueEx(hKey, strKeyName.c_str() , 0, regType, (LPBYTE) &unValue, dw); 
		if( err == ERROR_SUCCESS && RegCloseKey(hKey) == ERROR_SUCCESS )
		{
			return true;
		}	
		RegCloseKey( hKey );
	} 
	else if( err == ERROR_FILE_NOT_FOUND )
	{		
		if( CreateKey( hRegKey,strRegEntry) )
		{
			HKEY hKey;
			DWORD dw = 4; 
			LONG err = -1; 
			DWORD regType = REG_DWORD; 	

			err = RegOpenKeyEx(hRegKey, strRegEntry.c_str(),0, KEY_SET_VALUE, &hKey) ;
			if(err == ERROR_SUCCESS)
			{ 
				err = RegSetValueEx(hKey, strKeyName.c_str() , 0, regType, (LPBYTE) &unValue, dw); 
				if( err == ERROR_SUCCESS && RegCloseKey(hKey) == ERROR_SUCCESS )
				{
					return true;
				}	
				RegCloseKey( hKey );
			}
		}
	}

	return false;
}
bool YL_RegInfo::WriteDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,const unsigned int& unValue)
{
	if( pszRegEntry == NULL || pszKeyName == NULL )
	{
		return false;
	}
	return WriteDWORD( hRegKey,string(pszRegEntry),string(pszKeyName),unValue );
}


bool YL_RegInfo::ReadDWORD(HKEY hRegKey, const string &strRegEntry, const string& strKeyName , unsigned int& unValue)
{
	HKEY hKey; 
	DWORD dw = 4; 
	LONG err ; 
	DWORD regType = REG_DWORD;
	
	err = RegOpenKeyEx(hRegKey, strRegEntry.c_str() ,0, KEY_READ, &hKey);
	if(err == ERROR_SUCCESS) 
	{ 
		err = RegQueryValueEx(hKey, strKeyName.c_str() , 0, &regType,(LPBYTE) &unValue, &dw); 
		if( err == ERROR_SUCCESS && RegCloseKey(hKey) == ERROR_SUCCESS )
		{
			return true;
		}
		RegCloseKey( hKey );
	} 

	return false;
}

bool YL_RegInfo::ReadDWORD( HKEY hRegKey, const char* pszRegEntry, const char* pszKeyName,unsigned int& unValue)
{
	if( pszRegEntry == NULL || pszKeyName == NULL )
	{
		return false;
	}
	return ReadDWORD( hRegKey,string(pszRegEntry),string(pszKeyName),unValue );
}