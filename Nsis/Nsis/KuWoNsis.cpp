// KuWoNsis.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include <io.h>
//#include <fcntl.h>
//#include <Objidl.h>

#define WI_VT_ARRAY		0
#define WI_VT_BYREF		1
#define WI_VT_EMPTY		2
#define WI_VT_NULL		3
#define WI_VT_BLOB		4
#define WI_VT_BOOL		5
#define WI_VT_I2		6
#define WI_VT_I4		7
#define WI_VT_R4		8
#define WI_VT_R8		9
#define WI_VT_BSTR		10
#define WI_VT_LPSTR		11
#define WI_VT_FILETIME	12
#define WI_VT_CF		13
#define WI_UNHANDLED	14

#define FORCE_SWITCH  "/FORCE"
#define NOSAFE_SWITCH "/NOSAFE"
#define MAX_STRLEN       1024

//#include "gtapi.h"
#include <string>
using namespace std;

/* NSIS stack structure */
typedef struct _stack_t {
	struct	_stack_t *next;
	char		text[MAX_STRLEN];
} stack_t;

stack_t		  **g_stacktop;
char		   *g_variables;
unsigned int	g_stringsize;
HINSTANCE		g_hInstance;

#define EXDLL_INIT()					\
{										\
	g_stacktop		= stacktop;    \
	g_variables		= variables;   \
	g_stringsize	= string_size; \
}

//Function: Removes the element from the top of the NSIS stack and puts it in the buffer
int popstring(char *str)
{
	stack_t *th;

	if (!g_stacktop || !*g_stacktop) return 1;

	th=(*g_stacktop);
	lstrcpy(str,th->text);
	*g_stacktop = th->next;
	GlobalFree((HGLOBAL)th);

	return 0;
}

//Function: Adds an element to the top of the NSIS stack
void pushstring(const char *str)
{
	stack_t *th;

	if (!g_stacktop) return;

	th=(stack_t*)GlobalAlloc(GPTR, sizeof(stack_t)+g_stringsize);
	lstrcpyn(th->text,str,g_stringsize);
	th->next=*g_stacktop;

	*g_stacktop=th;
}

#include "Aclapi.h"
#include "winerror.h"

void ChangeRegKeyAccessRights()
{
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

	DWORD dwRes = GetNamedSecurityInfoA("MACHINE\\SOFTWARE\\YPLAYBOX",SE_REGISTRY_KEY,DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
	EXPLICIT_ACCESS ea;

	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));	
	BuildExplicitAccessWithNameA(&ea, "Everyone", KEY_ALL_ACCESS, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT); 
	
	// Create a new ACL that merges the new ACE
	// into the existing DACL.
	dwRes = SetEntriesInAclA(1, &ea, pOldDACL, &pNewDACL);

	// Attach the new ACL as the object's DACL.
	dwRes = SetNamedSecurityInfoA("MACHINE\\SOFTWARE\\YPLAYBOX",SE_REGISTRY_KEY,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);

	return;
}

void ChangeFileAccessRights(char *str_value)
{
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	DWORD dwRes = GetNamedSecurityInfo(str_value,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);

	EXPLICIT_ACCESS ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));	
	BuildExplicitAccessWithName(&ea, "Everyone", FILE_ALL_ACCESS, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT); 

	// Create a new ACL that merges the new ACE
	// into the existing DACL.
	dwRes = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);

	// Attach the new ACL as the object's DACL.
	dwRes = SetNamedSecurityInfo(str_value,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);
	return;
}

extern "C" __declspec(dllexport) void AddAccessRights(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
	char str_value	[MAX_STRLEN] = "0";
	EXDLL_INIT();
	{
		popstring(str_value);
		OSVERSIONINFO  version = {0};
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&version);

		//char szVersion[10];
		//memset( szVersion, 0, 10 );
		//itoa( int(version.dwMajorVersion), szVersion, 10 );
		//::MessageBox( hwndParent, str_value, szVersion, MB_OK );

		if(version.dwMajorVersion < 6)
			return;

		ChangeFileAccessRights(str_value);
		ChangeRegKeyAccessRights();
		return;
	}
}