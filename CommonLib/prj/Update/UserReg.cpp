#include "stdafx.h"
#include <Iphlpapi.h>
#include <math.h>
#include "UserReg.h"
#include "stdio.h"
#include "../../common/LhcImg.h"
#include "../../common/YL_RegInfo.h"
#include "../../common/YL_Base64.h"
#include "../../common/YL_UserId.h"
#include "../../include/YL_HTTPRequest.h"
#include "../Log/KwLogSvr.h"

#pragma warning( disable: 4800 )

MyMusic::MyMusic(void)
{
}

MyMusic::~MyMusic(void)
{
}

bool MyMusic::Encryption( char* szSrc, unsigned int iSrclength, char** szDest )
{
	bool bRet = false;
	try{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UserReg", "Encryption in, src is %s.", szSrc);
		int i = BASE64_LENGTH(iSrclength)+1;
		*szDest = new char[BASE64_LENGTH(iSrclength)+1];//Base64::Encode((unsigned char*) pTemp,iSrclength);
		if(*szDest != NULL)
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UserReg", "Encryption YL_Base64Encode in.");
			memset(*szDest, 0, BASE64_LENGTH(iSrclength)+1);
			if(YL_Base64Encode(*szDest, szSrc, iSrclength, "langhua ")>0)
				bRet = true;
		}
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "UserReg", "Encryption YL_Base64Encode out.");
	}
	catch(...)
	{
		return false;
	}
	return bRet;
}

bool MyMusic::Decryption( char* szSrc, char** szDest, unsigned int iLength )
{
	*szDest = new char[BASE64_LENGTH(iLength)+1];
	if(*szDest != NULL)
	{
		YL_Base64Decode((char*)*szDest, szSrc, "yeelion ");
		return true;
	}
	else
		return false;
}

bool MyMusic::GetLocalIP( char* szIP )
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD( 1, 1 );	

	char* szIPTemp;

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		if( gethostname ( name, sizeof(name)) == 0)
		{
			if( ((hostinfo = gethostbyname(name)) != NULL) && (hostinfo->h_length == 4) )
			{								
				szIPTemp = inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[0]);							
				if( szIPTemp != NULL )
				{
					char* pPos = szIPTemp;
					int i = 0;
					while( i < 4 )
					{
						pPos = strstr(szIPTemp,".");					
						if( pPos != NULL )	
						{
							*pPos = 0;
						}
						szIP[i++] = atoi( szIPTemp);
						int itemp = atoi( szIPTemp );
						szIPTemp = pPos+1;
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool MyMusic::GetLocalMACAddress( char* szMacAddr )
{
	IP_ADAPTER_INFO AdapterInfo[16];			// Allocate information for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);		// Save the memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo
		AdapterInfo,							// [out] buffer to receive data
		&dwBufLen);								// [in] size of receive data buffer
	if(dwStatus != ERROR_SUCCESS)			// Verify return value is valid, no buffer overflow
		return false;

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
	_snprintf( szMacAddr, 12, "%02X%02X%02X%02X%02X%02X", 
		pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], 
		pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5] );
	return true;
}

bool MyMusic::GetMacAddress(char *szMac)
{
	HKEY hKey, hKey2;
	char m_SubKey[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards", szName[MAX_PATH], szValue[MAX_PATH];
	DWORD cbNameLen = MAX_PATH, cbValueLen = MAX_PATH, dwIndex = 0, dwAttr = 0;
	if(RegOpenKeyEx(HKEY_PLAYBOX_ROOT, m_SubKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return false;

	BYTE mac[6];
	BOOL result = FALSE;
	while(RegEnumKeyEx(hKey, dwIndex, szName, &cbNameLen, 0, NULL, NULL, 0) != ERROR_NO_MORE_ITEMS)
	{
		if(strcmp(szName, "") != NULL)
		{
			//如果找到了，分别在枚举下面有没有ServiceName
			char szCard[MAX_PATH+1];
			_snprintf(szCard, MAX_PATH, "%s\\%s", m_SubKey, szName);
			//再打开，查找
			if(RegOpenKeyEx(HKEY_PLAYBOX_ROOT, szCard, 0, KEY_READ, &hKey2) == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey2,"ServiceName", 0, &dwAttr, LPBYTE(szValue), &cbValueLen);
				RegCloseKey(hKey2);
				szValue[cbValueLen] = 0;

				string strDriver = string("\\\\.\\") + szValue;
				HANDLE hICDriver = CreateFile(strDriver.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);   
				if( hICDriver != INVALID_HANDLE_VALUE )
				{
					ULONG32 oid = OID_802_3_PERMANENT_ADDRESS;       // Defined in Ntddpack.h
					DWORD dwByteCount = 0;
					result = DeviceIoControl(hICDriver, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, 4, mac, 6, &dwByteCount, NULL);
					CloseHandle(hICDriver);
					if(result != FALSE)
						break;
				}
			}
		}
		dwIndex++;
		cbNameLen = MAX_PATH;
		cbValueLen = MAX_PATH;
	}
	RegCloseKey(hKey);

	if(result == FALSE)
		return false;
	else
	{
		_snprintf( szMac, 12, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
		return true;
	}
}

bool MyMusic::GetOSVersion( char* szOSVersion )
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if( !GetVersionEx( &osinfo ) )
		return false;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	if( (dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4) )
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			memmove( szOSVersion,"   w95",6 );
		}
		else if ((dwMinorVersion < 10) && 
			((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			memmove( szOSVersion,"w95sp1",6 );
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			memmove( szOSVersion,"w95osr",6 );
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			memmove( szOSVersion,"   w98",6 );
		}
		else if ((dwMinorVersion == 10) && 
			((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			memmove( szOSVersion,"w98sp1",6 );
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			memmove( szOSVersion," w98se",6 );
		}
		else if (dwMinorVersion == 90)
		{
			memmove( szOSVersion," winme",6 );
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			memmove( szOSVersion,"wnt351",6 );
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			memmove( szOSVersion,"  wnt4",6 );
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			memmove( szOSVersion," win2k",6 );
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			memmove( szOSVersion," winxp",6 );
		}
	}
	else
	{
		memmove( szOSVersion,"unknow",6);
	}
	return true;
}

bool MyMusic::GetRamSize( char* szRamSize )
{
	MEMORYSTATUS memoryStatus;
	ZeroMemory(&memoryStatus,sizeof(MEMORYSTATUS));
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);

	::GlobalMemoryStatus (&memoryStatus);
	WORD wSize = (WORD) ceil((double)(memoryStatus.dwTotalPhys/1024/1024));	
	memmove( szRamSize,&wSize,2 );		
	return true;
}

bool MyMusic::GetCPUInfo( char* szCPUInfo )
{
	char szCPU[256];
	//HKEY hKey; 		
	DWORD regType = REG_DWORD; 
	DWORD dw = sizeof(DWORD);
	unsigned int dwCPUSpeed;
	szCPU[0] = 0;
	if (!YL_RegInfo::ReadDWORD(HKEY_PLAYBOX_ROOT, "Hardware\\Description\\System\\CentralProcessor\\0", "~MHz", dwCPUSpeed)) 
	{
		szCPU[0] = 0; 
	}

	YL_RegInfo::ReadString(HKEY_PLAYBOX_ROOT, "Hardware\\Description\\System\\CentralProcessor\\0", "VendorIdentifier", szCPU, 256);
	if( _stricmp( szCPU, "GenuineIntel" ) == 0 )
	{
		YL_RegInfo::ReadString(HKEY_PLAYBOX_ROOT,"Hardware\\Description\\System\\CentralProcessor\\0","ProcessorNameString",szCPU,256 );		
		if( strstr( szCPU,"Pentium(R) M" ) )
		{
			szCPUInfo[0] = 'M';
		}
		else if( strstr( szCPU,"Celeron") )
		{
			szCPUInfo[0] = 'C';
		}
		else
		{
			szCPUInfo[0] = 'P';
		}
	}
	else if( _stricmp( szCPU,"AuthenticAMD") == 0 )
	{
		szCPUInfo[0] = 'A';
	}
	else 
	{
		szCPUInfo[0] = 'U';
	}

	char szTmp[4];
	itoa( (int)((double)dwCPUSpeed / (double)100 + 0.5) ,szTmp,10 );
	if( strlen( szTmp) == 2 )
	{		
		memset( (void*)&szCPUInfo[1],' ',1 );
		memmove( (void*)&szCPUInfo[2],szTmp,2 );
	}
	else if( strlen( szTmp) == 1 )
	{
		memset( (void*)&szCPUInfo[1],' ',2 );
		memmove( (void*)&szCPUInfo[3],szTmp,1 );
	}
	else
	{		
		memmove( (void*)&szCPUInfo[1],szTmp,3 );
	}
	return true;
}

bool MyMusic::DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned )
{
	// Set up data structures for IDENTIFY command.
	pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg     = 0;
	pSCIP->irDriveRegs.bSectorCountReg  = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg       = 0;
	pSCIP->irDriveRegs.bCylHighReg      = 0;

	// calc the drive number.
	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );

	// The command can either be IDE identify or ATAPI identify.
	pSCIP->irDriveRegs.bCommandReg = bIDCmd;
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		( LPVOID ) pSCIP,
		sizeof( SENDCMDINPARAMS ) - 1,
		( LPVOID ) pSCOP,
		sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL );
}

bool MyMusic::WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
#define  DFP_GET_VERSION          0x00074080
	bool bInfoLoaded = false;

	for( UINT uDrive = 0; uDrive < 4; ++ uDrive )
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char szDriveName [256];
		_snprintf( szDriveName, 255, "\\\\.\\PhysicalDrive%d", uDrive );

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile( szDriveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
		{
			GETVERSIONOUTPARAMS VersionParams = { 0 };
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			if( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof( GETVERSIONOUTPARAMS ),
				&cbBytesReturned, NULL ) )
			{
				// If there is a IDE device at number "i" issue commands
				// to the device
				if( VersionParams.bIDEDeviceMap != 0 )
				{
					BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
					SENDCMDINPARAMS  scip = { 0 };

					// Now, get the ID sector for all IDE devices in the system.
					// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
					// otherwise use the IDE_ATA_IDENTIFY command
					bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
					BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };

					if( DoIdentify( hPhysicalDriveIOCTL, 
						&scip, 
						( PSENDCMDOUTPARAMS )&IdOutCmd, 
						( BYTE )bIDCmd,
						( BYTE )uDrive,
						&cbBytesReturned ) )
					{
						if( *puSerialLen + 20U <= uMaxSerialLen )
						{
							CopyMemory( dwSerial + *puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 );  // 序列号
							// Cut off the trailing blanks
							UINT i = 20;
							for( ; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; -- i )  {}
							*puSerialLen += i;

							//CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // 型号
							//// Cut off the trailing blanks
							//for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
							//*puSerialLen += i;

							bInfoLoaded = true;
							break;
						}
						else
						{
							::CloseHandle( hPhysicalDriveIOCTL );
							return bInfoLoaded;
						}
					}
				}
			}
			CloseHandle( hPhysicalDriveIOCTL );
		}
	}
	return bInfoLoaded;
}

bool MyMusic::GetHDiskSerial(char szDiskSerial[])
{
	bool bRet = false;

	OSVERSIONINFO ovi = { 0 };
	ovi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	BYTE szSystemInfo[1024]; // 在程序执行完毕后，此处存储取得的系统特征码
	UINT uSystemInfoLen = 0; // 在程序执行完毕后，此处存储取得的系统特征码的长度

	GetVersionEx( &ovi );
	if( ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
	{
		// Only Windows 2000, Windows XP, Windows Server 2003...
		return false;
	}
	else
	{
		UINT uSystemInfoLen = 0;
		bRet = WinNTHDSerialNumAsPhysicalRead( szSystemInfo, &uSystemInfoLen, 1024 );
		if(bRet)
		{
			int n = 0;
			for(int i=0; i<20 && szSystemInfo[i]!=0; i++)
			{
				if(szSystemInfo[i] != ' ')
				{
					szDiskSerial[n] = szSystemInfo[i];
					n++;
				}
			}
			szDiskSerial[n] = 0;
		}
	}
	return bRet;
}