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
  	Name  "浪花游戏盒 2010"
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
!define MUI_HEADERIMAGE #标题图片，可放产品/公司的图片。
!define MUI_HEADERINAGE_RIGHT #标题图片所在位置，左/右。
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
	!define PRODUCT_NAME  "浪花游戏盒"
	!define PRODUCT_SHOW_NAME  "浪花游戏盒 2010"
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
	MessageBox MB_OK "对不起，本软件只能安装在Windows2000或更高版本的操作系统中..."
	Abort
NoAbort:
	#保证安装程序单实例运行
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${NSIS_MUTEX}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	   MessageBox MB_OK|MB_ICONEXCLAMATION "安装程序或者卸载程序已经在运行。"
	   Abort
	   

	ReadIniStr $0 "$R1\res\pic\lhc.img" "config" "version"
	${If} $0 != ""
		strcpy $R0 $0 13 0
		${If} ${CUR_VERSION} S< $R0
			ifSilent +2 0
				MessageBox MB_OK "检测到您本地有更高版本的浪花游戏盒，如需安装本版游戏盒，$\n请先卸载本地游戏盒。'确定'退出安装程序。"
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
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 2" "Text" "欢迎使用 浪花游戏盒 升级程序"
	!insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 3" "Text" "欢迎使用浪花游戏盒升级程序。\r\n\r\n本程序将帮助您升级浪花游戏盒。\r\n\r\n\r\n最新更新：\r\n\r\n1.游戏大厅首页改版，热门新鲜游戏全收罗。\r\n\r\n2.优化游戏启动机制，玩家体验更优。\r\n\r\n3.全面升级游戏服务，软件运行更稳定。\r\n\r\n4.修改了一些bug。"
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
			messagebox::show MB_DEFBUTTON1 "" "" "安装程序需要先关闭正在运行的${PRODUCT_SHOW_NAME}软件才能继续安装" "继续" "取消"
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
	#记录升级前的版本号
	ReadIniStr $R0 "$INSTDIR\res\pic\lhc.img" "config" "version"
	WriteRegStr HKLM "${REG_UPDATE}" "Last_Version" "$R0"

#Output Files:
	StrCpy $R0 "$INSTDIR\LangHua.exe"
	Call RenameAndDeleteReboot
	File  ..\install_resource\LangHua.exe
	ExecShell "open" "Netsh.exe" 'firewall add allowedprogram "$INSTDIR\LangHua.exe" 浪花游戏盒 ENABLE' SW_HIDE
	File "/oname=$TEMP\config.ini" ..\install_resource\config.ini
	#不能覆盖用户配置
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
	
	#调试信息dll
	File  ..\install_resource\ex_dll.dll
	File  ..\install_resource\Encode.exe
	
	System::Call 'kernel32::GetModuleFileNameA(i 0, t .R0, i 1024) i r1'
	WriteRegStr HKLM "${REG_ROOT}\BHO" "First_Start" "$R0"
	WriteRegStr HKLM "${REG_ROOT}" "INSTALLED" "Success"
	
	#Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	
	#创建开始程序中单独快捷方式
	call GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext all
	${Else}
		#创建开始菜单快捷方式
		CreateShortCut "$STARTMENU\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	${EndIf}
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${MAIN_EXE_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\软件说明.lnk" "$INSTDIR\readme.txt"
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
	WriteRegStr HKLM $UninstPath "Publisher" "拓浪科技"
	WriteRegStr HKLM $UninstPath "VersionMajor" "${VERSION_NUM}"
	WriteRegStr HKLM $UninstPath "VersionMinor" "${VERSION_NUM}"

	DeleteRegValue HKLM ${REG_UPDATE} "Upload_Time"
	DeleteRegValue HKLM ${REG_ROOT} "NEW_LYRIC"
	DeleteRegValue HKLM ${REG_ROOT} "MB_NEW_LYRIC"
	#删除每天更新的服务器配置
	Delete "$INSTDIR\res\pic\svr.img"
	
	#删除启动程序标记,启动升级之前退出的程序
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
	#保证安装程序单实例运行
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${NSIS_MUTEX}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	   MessageBox MB_OK|MB_ICONEXCLAMATION "安装程序或者卸载程序已经在运行。"
	   Abort
FunctionEnd

Section "Uninstall"
	IntOp $R1 0 + 0
	sleep 200
	#判断界面是否正在运行
  	IntCmp $R1 0 +2
		System::Call 'kernel32::CloseHandle(i $R1) i .r2'
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${LANGHUA_MUTEX}") i .R1 ?e'
	Pop $R0
	StrCmp $R0 0 Un_Check_KwMusic_End
		messagebox::show MB_DEFBUTTON1 "" "" "卸载程序需要先关闭正在运行的${PRODUCT_SHOW_NAME}软件才能继续" "继续" "取消"
		pop $0
		${If} $0 == "1"
		    KillProcDLL::KillProc "${MAIN_EXE_NAME}"
		    goto Un_Check_KwMusic_End
		${EndIf}
		Quit
Un_Check_KwMusic_End:
	#删除已下载的flash
	ReadIniStr $0 "$INSTDIR\config.ini" "App" "SwfPath"
	${If} $0 != ""
		MessageBox MB_YESNO "是否删除$0目录下的flash文件?" IDYES 0 IDNO +1
		RMDir /r "$0"
	${EndIf}
	#发送卸载消息
	ExecWait '"$INSTDIR\Encode.exe" UNINSTALL'
	#删除日志已经上传的标记
	DeleteRegValue HKLM "${REG_ROOT}\UPDATE" "Upload_Time"

	Delete "$INSTDIR\Uninstall.exe"
	RMDir /r "$INSTDIR\update"
	RMDir /r "$INSTDIR\log"
	RMDir /r "$INSTDIR\res"
	#延迟删除可执行文件
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
	#删除桌面快捷方式
	Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
	call un.GetWindowsVersion
	Pop $R0
	${If} $R0 == "VISTA"
	${OrIf} $R0 == "7"
		SetShellVarContext all
	${EndIf}
	#删除开机启动快捷方式
	Delete "$SMSTARTUP\${PRODUCT_NAME}.lnk"
	#删除开始菜单快捷方式
	Delete "$STARTMENU\${PRODUCT_NAME}.lnk"
	#删除开始程序快捷方式
	Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
	Delete "$SMPROGRAMS\${PRODUCT_NAME}\软件说明.lnk"
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
