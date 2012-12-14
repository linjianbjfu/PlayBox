;NSIS Modern User Interface version 1.70

SetCompressor /SOLID /FINAL lzma
AutoCloseWindow true

!define MUI_ICON ..\install_resource\install.ico
!define MUI_UNICON ..\install_resource\uninstall.ico
;--------------------------------
#Include Modern UI
	!include "KwUtils.nsh"
  	!include "MUI.nsh"
  	!include "WinMessages.nsh"
	!include "TextFunc.nsh"
	!define LOGICLIB_STRCMP
	!include LogicLib.nsh
	!define NEW_SKIN_VER  "LANGHUA_3.0.0.0"
	!define VERSION_NUM   "1.0.1.0_U0"
;--------------------------------
  	Name  "�˻���Ϸ�� 2010"
  	OutFile "LangHua_force${VERSION_NUM}.exe"
  	
  	InstallDir "$PROGRAMFILES\LANGHUA"
;--------------------------------
#Variables
  	var MUI_TEMP
	var UninstPath
  	var OS
;--------------------------------
#ui setting
!define MUI_BGCOLOR "f0f0f0"
!define MUI_LICENSEPAGE_BGCOLOR "f0f0f0"
!define MUI_HEADERIMAGE #����ͼƬ���ɷŲ�Ʒ/��˾��ͼƬ��
!define MUI_HEADERINAGE_RIGHT #����ͼƬ����λ�ã���/�ҡ�
;!define MUI_HEADERIMAGE_BITMAP ..\install_resource\header150X53.bmp
;!define MUI_HEADERIMAGE_UNBITMAP ..\install_resource\header150X53.bmp
!define MUI_INSTFILESPAGE_COLORS "/windows"

!define MUI_INSTALLCOLORS "/windows"
;!define MUI_WELCOMEFINISHPAGE_BITMAP ..\install_resource\header150X53.bmp
;!define MUI_UNWELCOMEFINISHPAGE_BITMAP ..\install_resource\header150X53.bmp

#Interface Settings
  	!define REG_ROOT      "SOFTWARE\LANGHUA"
  	!define REG_UPDATE    "SOFTWARE\LANGHUA\UPDATE"
  	!define SOFR_HOME_KEY "LANGHUA_HOME"
  	!define MAIN_EXE_NAME "Langhua.exe"
	!define PRODUCT_NAME  "�˻���Ϸ��"
	!define PRODUCT_SHOW_NAME  "�˻���Ϸ�� 2010"
  	!define CUR_VERSION   "LANGHUA_${VERSION_NUM}"
  	!define NSIS_MUTEX    "LANGHUA_GBOX_INSTALL_MUTEX"
  	!define LANGHUA_MUTEX "LANGHUA_GameBox_2010-2-7"
  	!define MUI_ABORTWARNING
;--------------------------------
#Pages

Function .onInit
	Call GetWindowsVersion
	Pop $R0
	Strcpy $OS $R0
	StrCmp $OS "2000" NoAbort
	StrCmp $OS "XP" NoAbort
	StrCmp $OS "2003" NoAbort
	StrCmp $OS "VISTA" NoAbort
	StrCmp $OS "7" NoAbort
	MessageBox MB_OK "�Բ��𣬱����ֻ�ܰ�װ��Windows2000����߰汾�Ĳ���ϵͳ��..."
	Abort
NoAbort:
	#��֤��װ����ʵ������
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${NSIS_MUTEX}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	   MessageBox MB_OK|MB_ICONEXCLAMATION "��װ�������ж�س����Ѿ������С�"
	   Abort
	   

	ReadIniStr $0 "$R1\res\pic\lhc.img" "config" "version"
	${If} $0 != ""
		strcpy $R0 $0 13 0
		${If} ${CUR_VERSION} S< $R0
			ifSilent +2 0
				MessageBox MB_OK "��⵽�������и��߰汾���˻���Ϸ�У����谲װ������Ϸ�У�$\n����ж�ر�����Ϸ�С�'ȷ��'�˳���װ����"
				Abort
		${EndIf}
	${EndIf}
	ClearErrors
	ReadRegStr $INSTDIR HKLM "${REG_ROOT}" "${SOFR_HOME_KEY}"
	ifErrors 0 +2
		StrCpy $INSTDIR "$PROGRAMFILES\LANGHUA"
FunctionEnd
	!define MUI_PAGE_CUSTOMFUNCTION_PRE Pre_PAGE_WELCOME
	!insertmacro MUI_PAGE_WELCOME
Function Pre_PAGE_WELCOME
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "CancelShow" "0"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 2" "Text" "��ӭʹ�� �˻���Ϸ�� ��������"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 3" "Text" "��ӭʹ���˻���Ϸ����������\r\n\r\n�����򽫰����������˻���Ϸ�С�\r\n\r\n\r\n���¸��£�\r\n\r\n1.��Ϸ������ҳ�İ棬����������Ϸȫ���ޡ�\r\n\r\n2.�Ż���Ϸ�������ƣ����������š�\r\n\r\n3.ȫ��������Ϸ����������и��ȶ���\r\n\r\n4.�޸���һЩbug��"
FunctionEnd

	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH
;--------------------------------
#Languages
	!insertmacro MUI_LANGUAGE "SimpChinese"
	SpaceTexts none
	ShowInstDetails nevershow
	BrandingText /TRIMRIGHT  "www.7k7k.com"

;--------------------------------
#Installer Sections
Section "-LangHua" realSection
	sleep 2000
	SetOutPath "$INSTDIR"
	;File  ..\install_resource\instpage.gif
	;AnimGif::play /NOUNLOAD "$INSTDIR\instpage.gif"
	IntOp $R1 0 + 0
	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${LANGHUA_MUTEX}") i .R1 ?e'
	Pop $R0
	StrCmp $R0 0 Check_KwMusic_End
		ifSilent Quit_For_Check_KwMusic 0
			messagebox::show MB_DEFBUTTON1 "" "" "��װ������Ҫ�ȹر��������е�${PRODUCT_SHOW_NAME}������ܼ�����װ" "����" "ȡ��"
			pop $0
			${If} $0 == "1"
			    KillProcDLL::KillProc "${MAIN_EXE_NAME}"
			    goto Check_KwMusic_End
			${EndIf}
Quit_For_Check_KwMusic:
		WriteRegStr HKLM "${REG_ROOT}" "INSTALLED" "PLAYER_RUNNING"
		Quit
Check_KwMusic_End:
	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	sleep 1000

	SetOutPath "$INSTDIR"
	WriteRegStr HKLM "${REG_ROOT}" "${SOFR_HOME_KEY}" "$INSTDIR"
	#��¼����ǰ�İ汾��
	ReadIniStr $R0 "$INSTDIR\res\pic\lhc.img" "config" "version"
	WriteRegStr HKLM "${REG_UPDATE}" "Last_Version" "$R0"

#Output Files:
	StrCpy $R0 "$INSTDIR\LangHua.exe"
	Call RenameAndDeleteReboot
	File  ..\install_resource\LangHua.exe
	ExecShell "open" "Netsh.exe" 'firewall add allowedprogram "$INSTDIR\LangHua.exe" �˻���Ϸ�� ENABLE' SW_HIDE
	File "/oname=$TEMP\config.ini" ..\install_resource\config.ini
	#���ܸ����û�����
	IfFileExists "$INSTDIR\config.ini" +2 0
		File  ..\install_resource\config.ini
		
	File  ..\install_resource\readme.txt
	###########################
	File  /r /x cvs /x *.db ..\install_resource\res
 	WriteINIStr "$INSTDIR\res\pic\lhc.img" "config" "version" "${CUR_VERSION}"
	File  /r /x cvs /x *.db /x *.nouse ..\install_resource\skin
	Call KeepSkinSub
	CreateDirectory "$INSTDIR\log"
	CreateDirectory "$INSTDIR\update"
	##################################################################################
	SetOutPath "$INSTDIR"
	IfFileExists "$INSTDIR\MSVCR71.DLL" +2 0
		File  "..\install_resource\MSVCR71.DLL"
	IfFileExists "$INSTDIR\MSVCP71.DLL" +2 0
		File  "..\install_resource\MSVCP71.DLL"
	IfFileExists "$INSTDIR\MFC71.dll" +2 0
		File  "..\install_resource\MFC71.dll"

	StrCpy $R0 "$INSTDIR\LhLogSvr.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\LhLogSvr.dll
	
	StrCpy $R0 "$INSTDIR\LhUpdate.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\LhUpdate.dll
	
	StrCpy $R0 "$INSTDIR\FZip.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\FZip.dll
	
	StrCpy $R0 "$INSTDIR\HTTP.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\HTTP.dll
	
	StrCpy $R0 "$INSTDIR\Zlib.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\Zlib.dll
	
	#������Ϣdll
	File  ..\install_resource\ex_dll.dll
	File  ..\install_resource\Encode.exe
	
	System::Call 'kernel32::GetModuleFileNameA(i 0, t .R0, i 1024) i r1'
	WriteRegStr HKLM "${REG_ROOT}\BHO" "First_Start" "$R0"
	WriteRegStr HKLM "${REG_ROOT}" "INSTALLED" "Success"
	
	#Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	#������ʼ�����е�����ݷ�ʽ
	call GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext all
	${Else}
		#������ʼ�˵���ݷ�ʽ
		CreateShortCut "$STARTMENU\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	${EndIf}
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\���˵��.lnk" "$INSTDIR\readme.txt"
	call GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext current
	${EndIf}
	CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	CreateShortCut "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"

	strCpy $UninstPath "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\LangHua"
	WriteRegStr HKLM $UninstPath "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM $UninstPath "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM $UninstPath "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM $UninstPath "Publisher" "���˿Ƽ�"
	WriteRegStr HKLM $UninstPath "VersionMajor" "${VERSION_NUM}"
	WriteRegStr HKLM $UninstPath "VersionMinor" "${VERSION_NUM}"

	DeleteRegValue HKLM ${REG_UPDATE} "Upload_Time"
	DeleteRegValue HKLM ${REG_ROOT} "NEW_LYRIC"
	DeleteRegValue HKLM ${REG_ROOT} "MB_NEW_LYRIC"
	#ɾ��ÿ����µķ���������
	Delete "$INSTDIR\res\pic\svr.img"
	
	#ɾ������������,��������֮ǰ�˳��ĳ���
	StrCpy $0 ""
	ReadRegStr $0 HKLM ${REG_ROOT} "PLAYERCMD"
	${If} $0 != ""
		DeleteRegValue HKLM ${REG_ROOT} "PLAYERCMD"
	${EndIf}
	ExecShell "open" "$INSTDIR\readme.txt"
	Exec "$INSTDIR\${MAIN_EXE_NAME}"
	SetRebootFlag false
	;AnimGif::stop
SectionEnd

Function un.onInit
	#��֤��װ����ʵ������
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${NSIS_MUTEX}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	   MessageBox MB_OK|MB_ICONEXCLAMATION "��װ�������ж�س����Ѿ������С�"
	   Abort
FunctionEnd

Section "Uninstall"
	IntOp $R1 0 + 0
	sleep 200
	#�жϽ����Ƿ���������
  	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${LANGHUA_MUTEX}") i .R1 ?e'
	Pop $R0
	StrCmp $R0 0 Un_Check_KwMusic_End
		messagebox::show MB_DEFBUTTON1 "" "" "ж�س�����Ҫ�ȹر��������е�${PRODUCT_SHOW_NAME}������ܼ���" "����" "ȡ��"
		pop $0
		${If} $0 == "1"
		    KillProcDLL::KillProc "${MAIN_EXE_NAME}"
		    goto Un_Check_KwMusic_End
		${EndIf}
		Quit
Un_Check_KwMusic_End:
	#ɾ�������ص�flash
	ReadIniStr $0 "$INSTDIR\config.ini" "App" "SwfPath"
	${If} $0 != ""
		MessageBox MB_YESNO "�Ƿ�ɾ��$0Ŀ¼�µ�flash�ļ�?" IDYES 0 IDNO +1
		RMDir /r "$0"
	${EndIf}
	#����ж����Ϣ
	ExecWait '"$INSTDIR\Encode.exe" UNINSTALL'
	#ɾ����־�Ѿ��ϴ��ı��
	DeleteRegValue HKLM "${REG_ROOT}\UPDATE" "Upload_Time"

	Delete "$INSTDIR\Uninstall.exe"
	RMDir /r "$INSTDIR\update"
	RMDir /r "$INSTDIR\log"
	RMDir /r "$INSTDIR\res"
	#�ӳ�ɾ����ִ���ļ�
	StrCpy $R0 "$INSTDIR\LangHua.exe"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\LhUpdate.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\LhLogSvr.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\FZip.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\HTTP.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\Zlib.dll"
	Call un.RenameAndDeleteReboot
	!insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
	#ɾ�������ݷ�ʽ
	Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
	call un.GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext all
	${EndIf}
	#ɾ������������ݷ�ʽ
	Delete "$SMSTARTUP\${PRODUCT_NAME}.lnk"
	#ɾ����ʼ�˵���ݷ�ʽ
	Delete "$STARTMENU\${PRODUCT_NAME}.lnk"
	#ɾ����ʼ�����ݷ�ʽ
	Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\���˵��.lnk"
	Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
	RMDir  "$SMPROGRAMS\${PRODUCT_NAME}\"
	#Delete empty start menu parent diretories
	StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
startMenuDeleteLoop:
	    RMDir $MUI_TEMP
	    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
	    IfErrors startMenuDeleteLoopDone
	    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
startMenuDeleteLoopDone:
	call un.GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext current
	${EndIf}
	#Delete Uninstall Key
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\LangHua"
	DeleteRegKey HKLM "${REG_ROOT}"
	RMDir /r /REBOOTOK "$INSTDIR"

	SetRebootFlag false
SectionEnd
