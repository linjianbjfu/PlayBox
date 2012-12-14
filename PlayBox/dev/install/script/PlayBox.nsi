;NSIS Modern User Interface version 1.70

SetCompressor /SOLID /FINAL lzma
!define MUI_ICON ..\install_resource\install.ico
!define MUI_UNICON ..\install_resource\uninstall.ico
;--------------------------------
#Include Modern UI
	!include "KwUtils.nsh"
  	!include "MUI.nsh"
  	!include "WinMessages.nsh"
	!include "TextFunc.nsh"
	!insertmacro un.DirState
	!define LOGICLIB_STRCMP
	!include LogicLib.nsh
	!define VERSION_NUM  "0.0.3"
;--------------------------------
  	Name  "�ʰ���Ϸ"
  	OutFile "PlayBox${VERSION_NUM}SetUp.exe"
  	InstallDir "$PROGRAMFILES\YoungPlayBox"
;--------------------------------
#Variables
  	var MUI_TEMP
	var UninstPath
  	var OS
  	;--------------------------
  	;���ع�������
  	Var TMPSTR
  	Var IncreaseV
  	Var IncreaseTextHeight
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
  	!define REG_ROOT      "SOFTWARE\YPLAYBOX"
  	!define SOFR_HOME_KEY "path"
  	!define SOFR_VER_KEY  "ver"
  	!define MAIN_EXE_NAME "PlayBox.exe"
	!define PRODUCT_NAME  "�ʰ���Ϸ"
  	!define CUR_VERSION   "YPLAYBOX_${VERSION_NUM}"
  	!define NSIS_MUTEX    "YPLAYBOX_INSTALL_MUTEX"
  	!define YPLAYBOX_MUTEX "YPLAYBOX_2011_3_3" ;������д�������ͬ��
  	!define MUI_ABORTWARNING
;--------------------------------
#Pages
  	;!insertmacro MUI_PAGE_WELCOME
  	;!insertmacro MUI_PAGE_LICENSE "..\install_resource\license.txt"
  	!define MUI_PAGE_CUSTOMFUNCTION_SHOW MyFuncDir
  	!insertmacro MUI_PAGE_DIRECTORY

Function MyFuncDir
	#�ж��Ƿ��Ѿ���װ��
	ClearErrors
	ReadRegStr $INSTDIR HKLM "${REG_ROOT}" "${SOFR_HOME_KEY}"
	ifErrors MyFuncDirEnd 0
	IfFileExists "$INSTDIR\${MAIN_EXE_NAME}" 0 MyFuncDirEnd

	#���ƴ�����ʾ״̬
	FindWindow $R0 "#32770" "" $HWNDPARENT
	GetDlgItem $R1 $R0 1006
	SendMessage $R1 ${WM_SETTEXT} 0 "STR:"

	FindWindow $R0 "#32770" "" $HWNDPARENT
	GetDlgItem $R1 $R0 1019
	EnableWindow $R1 0
	SendMessage $R1 ${WM_SETTEXT} 0 "STR:$INSTDIR"

	FindWindow $R0 "#32770" "" $HWNDPARENT
	GetDlgItem $R1 $R0 1001
	EnableWindow $R1 0
	
	GetDlgItem $R1 $HWNDPARENT 1
	SendMessage $R1 ${WM_ACTIVATE} 2 0
MyFuncDirEnd:
	StrCpy $param $INSTDIR
FunctionEnd

Function .onInit
	Call GetWindowsVersion
	Pop $R0
	Strcpy $OS $R0
	StrCmp $OS "2000" NoAbort
	StrCmp $OS "XP" NoAbort
	StrCmp $OS "2003" NoAbort
	StrCmp $OS "VISTA" NoAbort
	StrCmp $OS "7" NoAbort
	MessageBox MB_OK "�뽫����windows�汾������Win2000����"
	Abort
NoAbort:
	#��֤��װ����ʵ������
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${NSIS_MUTEX}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	   MessageBox MB_OK|MB_ICONEXCLAMATION "��װ(ж��)��������������"
	   Abort

	ReadRegStr $0 HKLM "${REG_ROOT}" "${SOFR_VER_KEY}"
	${If} $0 != ""
		strcpy $R0 $0 13 0
		${If} ${VERSION_NUM} S< $R0
			ifSilent +2 0
				MessageBox MB_OK "��⵽�������и��߰汾�����$\n'ȷ��'�˳���װ����"
				Abort
		${EndIf}
	${EndIf}
FunctionEnd

	!insertmacro MUI_PAGE_INSTFILES
	!define MUI_FINISHPAGE_RUN ""
	!define MUI_PAGE_CUSTOMFUNCTION_PRE Pre_PAGE_FINISH
	!define MUI_PAGE_CUSTOMFUNCTION_SHOW Show_PAGE_FINISH
	!define MUI_PAGE_CUSTOMFUNCTION_LEAVE Leave_PAGE_FINISH
	!insertmacro MUI_PAGE_FINISH
###############################
#��������Pre_PAGE_FINISH
#���ܣ����������ҳ��ǰ�ĺ���
Function Pre_PAGE_FINISH
	StrCpy $TMPSTR "85"
	StrCpy $IncreaseV "2"
	StrCpy $IncreaseTextHeight "10"
	
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "NumFields" "5"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "CancelEnabled" "0"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Type" "CheckBox"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "State" "1"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Flags" "NOTIFY"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Left" "120"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Right" "315"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Top" "$TMPSTR"
	IntOp $TMPSTR $TMPSTR + $IncreaseTextHeight
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Bottom" "$TMPSTR"
	IntOp $TMPSTR $TMPSTR + $IncreaseV
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 4" "Text" "��������${PRODUCT_NAME}"

	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Type" "CheckBox"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "State" "1"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Flags" "NOTIFY"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Left" "120"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Right" "315"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Top" "$TMPSTR"
	IntOp $TMPSTR $TMPSTR + $IncreaseTextHeight
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Bottom" "$TMPSTR"
	IntOp $TMPSTR $TMPSTR + $IncreaseV
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 5" "Text" "�����Զ�����${PRODUCT_NAME}"
FunctionEnd

###############################
#��������Show_PAGE_FINISH
#���ܣ���ʾ����ҳ��ʱ�ĺ���
Function Show_PAGE_FINISH
	ReadINIStr $0 "$PLUGINSDIR\ioSpecial.ini" "Field 4" "HWND"
	${If} $0 != ""
		SetCtlColors $0 "" "${MUI_BGCOLOR}"
	${EndIf}

	ReadINIStr $0 "$PLUGINSDIR\ioSpecial.ini" "Field 5" "HWND"
	${If} $0 != ""
		SetCtlColors $0 "" "${MUI_BGCOLOR}"
	${EndIf}
FunctionEnd

###############################
#��������Leave_PAGE_FINISH
#���ܣ���ʾ����ҳ��ǰ�ĺ���
Function Leave_PAGE_FINISH
	ReadINIStr $0 "$PLUGINSDIR\ioSpecial.ini" "Settings" "State" ;��ȡ�Ǹ��ؼ�����ʹ�� state ���� Field ���������
	StrCmp $0 0 NextBtn  ; �����һ����ť
	Abort    ; ��ֹ������һҳ�棬��Ϊ����Ĳ����ǡ���һ������ť
NextBtn:
	ReadINIStr $0 "$PLUGINSDIR\ioSpecial.ini" "Field 5" "State"  ; ��ȡcheckbox ״̬
	Delete "$SMSTARTUP\${PRODUCT_NAME}.exe"
	#ɾ�������ݷ�ʽ
	Delete "$DESKTOP\${PRODUCT_NAME}.exe"
	Delete "$SMSTARTUP\${PRODUCT_NAME}.exe"
	${If} $0 == "1" ;ѡ���˿�������
		File  "/oname=$SMSTARTUP\${PRODUCT_NAME}.exe" "..\install_resource\run.exe"
	${Else}
	   	Delete "$SMSTARTUP\${PRODUCT_NAME}.exe"
	${EndIf}
	;�Ƿ�����
	ReadINIStr $0 "$PLUGINSDIR\ioSpecial.ini" "Field 4" "State"
	${If} $0 == "1"
		Exec "$INSTDIR\${MAIN_EXE_NAME}"
	${EndIf}
FunctionEnd

	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH
;--------------------------------
#Languages
	!insertmacro MUI_LANGUAGE "SimpChinese"
	SpaceTexts none
	ShowInstDetails nevershow
	BrandingText /TRIMRIGHT  "" ;����д��վ����
;--------------------------------
#Installer Sections
Section "-YPLAYBOX" realSection
	CALL Check_INST_DIRECTORY
	SetOutPath "$INSTDIR"
	###########��copy tool.exe�������ļ�����Ŀ¼
	StrCpy $R0 "$INSTDIR\NetOp.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\NetOp.dll

	File  ..\install_resource\Tool.exe
	File  /r /x .svn /x *.db ..\install_resource\Resources
	
	File  ..\install_resource\instpage.gif
	AnimGif::play /NOUNLOAD "$INSTDIR\instpage.gif"
	IntOp $R1 0 + 0
	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${YPLAYBOX_MUTEX}") i .R1 ?e'
	Pop $R0
	StrCmp $R0 0 Check_KwMusic_End
		ifSilent Quit_For_Check_KwMusic 0
			messagebox::show MB_DEFBUTTON1 "" "" "��װ������ر��������еı�������ܼ�����װ" "����" "ȡ��"
			pop $0
			${If} $0 == "1"
			    KillProcDLL::KillProc "${MAIN_EXE_NAME}"
			    goto Check_KwMusic_End
			${EndIf}
Quit_For_Check_KwMusic:
		#WriteRegStr HKLM "${REG_ROOT}" "INSTALLED" "PLAYER_RUNNING"
		ExecWait '"$INSTDIR\Tool.exe" WRITE ${REG_ROOT} INSTALLED PLAYER_RUNNING'
		Quit
Check_KwMusic_End:
	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	sleep 1000

	SetOutPath "$INSTDIR"
	WriteRegStr HKLM "${REG_ROOT}" "${SOFR_HOME_KEY}" "$INSTDIR"
	WriteRegStr HKLM "${REG_ROOT}" "${SOFR_VER_KEY}" "${VERSION_NUM}"
#Output Files:
	StrCpy $R0 "$INSTDIR\${MAIN_EXE_NAME}"
	Call RenameAndDeleteReboot
	File  ..\install_resource\${MAIN_EXE_NAME}
	ExecShell "open" "Netsh.exe" 'firewall add allowedprogram "$INSTDIR\${MAIN_EXE_NAME}" ${PRODUCT_NAME} ENABLE' SW_HIDE
	push $INSTDIR
	Nsis::AddAccessRights
	###########################
	#�Ѱ汾��д��config.ini
 	#WriteINIStr "$INSTDIR\res\pic\lhc.img" "config" "version" "${CUR_VERSION}"
 	ExecWait '"$INSTDIR\Tool.exe" WRITE config version ${CUR_VERSION}'
 	
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

	StrCpy $R0 "$INSTDIR\Log.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\Log.dll
	
	StrCpy $R0 "$INSTDIR\Update.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\Update.dll
	
	StrCpy $R0 "$INSTDIR\DocOp.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\DocOp.dll
	
	StrCpy $R0 "$INSTDIR\Zlib.dll"
	Call RenameAndDeleteReboot
	File  ..\install_resource\Zlib.dll
	File  ..\install_resource\run.exe
	####################################
	System::Call 'kernel32::GetModuleFileNameA(i 0, t .R0, i 1024) i r1'
	#WriteRegStr HKLM "${REG_ROOT}\BHO" "First_Start" "$R0"
	ExecWait '"$INSTDIR\Tool.exe" WRITE ${REG_ROOT}\BHO First_Start $R0'
	#WriteRegStr HKLM "${REG_ROOT}" "INSTALLED" "Success"
	ExecWait '"$INSTDIR\Tool.exe" WRITE ${REG_ROOT} INSTALLED Success'
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	#������ʼ�����е�����ݷ�ʽ
	call GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext all
	${Else}
		#������ʼ�˵���ݷ�ʽ
		#CreateShortCut "$STARTMENU\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	${EndIf}
	#CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	#CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	#CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	call GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext current
	${EndIf}
	#CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	#CreateShortCut "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"

	strCpy $UninstPath "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\YPLAYBOX"
	WriteRegStr HKLM $UninstPath "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegStr HKLM $UninstPath "DisplayName" "${PRODUCT_NAME}"
	WriteRegStr HKLM $UninstPath "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM $UninstPath "Publisher" ""
	WriteRegStr HKLM $UninstPath "VersionMajor" "${VERSION_NUM}"
	WriteRegStr HKLM $UninstPath "VersionMinor" "${VERSION_NUM}"

	#DeleteRegValue HKLM ${REG_ROOT} "NEW_LYRIC"
	ExecWait '"$INSTDIR\Tool.exe" DEL ${REG_ROOT} NEW_LYRIC'
	#DeleteRegValue HKLM ${REG_ROOT} "MB_NEW_LYRIC"
	ExecWait '"$INSTDIR\Tool.exe" DEL ${REG_ROOT} MB_NEW_LYRIC'
	#ɾ����������������
	#DeleteRegValue HKLM ${REG_ROOT} "PLAYERCMD"
	ExecWait '"$INSTDIR\Tool.exe" DEL ${REG_ROOT} PLAYERCMD'

	SetRebootFlag false
	AnimGif::stop
	Delete $INSTDIR\instpage.gif
SectionEnd

Function un.onInit
	#��֤��װ����ʵ������
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${NSIS_MUTEX}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	   MessageBox MB_OK|MB_ICONEXCLAMATION "��װ(ж��)��������������"
	   Abort
FunctionEnd

Section "Uninstall"
	IntOp $R1 0 + 0
	sleep 200
	#�жϽ����Ƿ���������
  	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${YPLAYBOX_MUTEX}") i .R1 ?e'
	Pop $R0
	StrCmp $R0 0 Un_Check_KwMusic_End
		messagebox::show MB_DEFBUTTON1 "" "" "ж�س�����Ҫ�ȹر��������еı�������ܼ���" "����" "ȡ��"
		pop $0
		${If} $0 == "1"
		    KillProcDLL::KillProc "${MAIN_EXE_NAME}"
		    goto Un_Check_KwMusic_End
		${EndIf}
		Quit
Un_Check_KwMusic_End:
	#����ж����Ϣ
	ExecWait '"$INSTDIR\Tool.exe" UNINSTALL'
	Delete "$INSTDIR\Uninstall.exe"
	RMDir /r "$INSTDIR\update"
	RMDir /r "$INSTDIR\log"
	RMDir /r "$INSTDIR\Resources"

	#�ӳ�ɾ����ִ���ļ�
	StrCpy $R0 "$INSTDIR\${MAIN_EXE_NAME}"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\Update.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\Log.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\DocOp.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\NetOp.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\Zlib.dll"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\Tool.exe"
	Call un.RenameAndDeleteReboot
	StrCpy $R0 "$INSTDIR\run.exe"
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
	Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
	RMDir  "$SMPROGRAMS\${PRODUCT_NAME}\"
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
	
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\YPLAYBOX"
	DeleteRegKey HKLM "${REG_ROOT}"
	RMDir /r /REBOOTOK "$INSTDIR"
	SetRebootFlag false
SectionEnd
