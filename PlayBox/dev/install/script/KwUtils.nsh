;常用函数头文件
!include "FileFunc.nsh"
!include LogicLib.nsh
!include UsefulLib.nsh
!insertmacro GetSize

var param
var loc_var
###############################
#函数名：JudgeUpdate
#输入参数：$R0:安装路径
#效果：如果是升级$R1里是1
#       否则$R1里是0
Function JudgeUpdate
	System::Call 'kernel32::GetModuleFileNameA(i 0, t .R2, i 1024) i r1'
  	${StrStrI} $0 "$R2" "$R0"
 	${If} $0 == ""
		StrCpy $R1 "0"
  	${Else}
		;是否已经安装过了
		IfFileExists "$R0\Tool.exe" hasInstalled notInstalled
notInstalled:
      	StrCpy $R1 "0"
      	Goto installedEnd
hasInstalled:
	  	;将安装目录记录下来
      	StrCpy $R1 "1"
	  	;升级分两种，静默和出进度条
		${StrStrI} $0 $R0 "Silent"
		${If} $0 == "" ;出进度条
	      	SetSilent normal
		${Else} ;静默升级
		  	SetSilent silent
		${EndIf}
installedEnd:
  	${EndIf}
FunctionEnd

Function Check_INST_DIRECTORY
	StrCmp $param $INSTDIR CHECK_DIRECTORY_END 0
	${GetSize} "$INSTDIR" "" $0 $1 $2
	${If} $1 <> ""
	${OrIf} $2 <> ""
	    StrCpy $loc_var $INSTDIR 7 -7
	    StrCmpS $loc_var "YoungPlayBox" CHECK_DIRECTORY_END 0

		StrCpy $loc_var $INSTDIR
		StrCpy $INSTDIR "$loc_var\YoungPlayBox"
	${EndIf}
CHECK_DIRECTORY_END:
FunctionEnd

###############################
#函数名：RenameAndDeleteReboot
#输入参数：$R0中为要改名的文件绝对路径
#效果：将$R0中的文件名修改为$R0.0.tmp，
#	   如果$R0.0.tmp存在则改名为$R0.1.tmp
#      如果$R0.1.tmp存在则改名为$R0.2.tmp
#	   照此方法类推
#      新文件名保存在$R1.
Function RenameFile
	;$R0原始文件名
	IntOp $R2 0 + 0 ;$R2记数
	StrCpy $R1 "$R0.$R2.tmp" ;$R1修改后的文件名
	IfFileExists $R1 exist notExist
exist:
	IntOp $R2 $R2 + 1
	StrCpy $R1 "$R0.$R2.tmp"
	IfFileExists $R1 exist notExist
notExist:
	Rename $R0 $R1
FunctionEnd


###############################
#函数名：RenameAndDeleteReboot
#输入参数：$R0中为要改名的文件绝对路径
#效果：将$R0中的文件名修改为$R0.0.tmp，
#	   如果$R0.0.tmp存在则改名为$R0.1.tmp
#      如果$R0.1.tmp存在则改名为$R0.2.tmp
#	   照此方法类推
Function RenameAndDeleteReboot
	;$R0原始文件名
	IntOp $R2 0 + 0 ;$R2记数
	StrCpy $R1 "$R0.$R2.tmp" ;$R1修改后的文件名
	IfFileExists $R1 exist notExist
exist:
	IntOp $R2 $R2 + 1
	StrCpy $R1 "$R0.$R2.tmp"
	IfFileExists $R1 exist notExist
notExist:
	Rename $R0 $R1
	Delete /REBOOTOK $R1
FunctionEnd

###############################
#函数名：RenameAndDeleteReboot
#输入参数：$R0中为要改名的文件绝对路径
#效果：将$R0中的文件名修改为$R0.0.tmp，
#	   如果$R0.0.tmp存在则改名为$R0.1.tmp
#      如果$R0.1.tmp存在则改名为$R0.2.tmp
#	   照此方法类推
Function un.RenameAndDeleteReboot
	;$R0原始文件名
	IntOp $R2 0 + 0 ;$R2记数
	StrCpy $R1 "$R0.$R2.tmp" ;$R1修改后的文件名
	IfFileExists $R1 exist notExist
exist:
	IntOp $R2 $R2 + 1
	StrCpy $R1 "$R0.$R2.tmp"
	IfFileExists $R1 exist notExist
notExist:
	Rename $R0 $R1
	Delete /REBOOTOK $R1
FunctionEnd


###############################
#函数名：GetWindowsVersion
# 返回到堆栈顶
#
# Windows 版本 (95, 98, ME, NT x.x, 2000, XP, 2003)
# 或
# '' (未知 Windows 版本)
#
# 用法:
#   Call GetWindowsVersion
#   Pop $R0
# 该函数来源于帮助文档
!macro GetWindowsVersion un
Function ${un}GetWindowsVersion
   Push $R0
   Push $R1
   ClearErrors
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
   IfErrors 0 lbl_winnt
   ; 不是 NT
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
   StrCpy $R1 $R0 1
   StrCmp $R1 '4' 0 lbl_error
   StrCpy $R1 $R0 3
   StrCmp $R1 '4.0' lbl_win32_95
   StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
   lbl_win32_95:
     StrCpy $R0 '95'
   Goto lbl_done
   lbl_win32_98:
     StrCpy $R0 '98'
   Goto lbl_done
   lbl_win32_ME:
     StrCpy $R0 'ME'
   Goto lbl_done

   lbl_winnt:
   StrCpy $R1 $R0 1
   StrCmp $R1 '3' lbl_winnt_x
   StrCmp $R1 '4' lbl_winnt_x
   StrCpy $R1 $R0 3
   StrCmp $R1 '5.0' lbl_winnt_2000
   StrCmp $R1 '5.1' lbl_winnt_XP
   StrCmp $R1 '5.2' lbl_winnt_2003
   StrCmp $R1 '6.0' lbl_winnt_vista
   StrCmp $R1 '6.1' lbl_winnt_7  lbl_error

   lbl_winnt_x:
     StrCpy $R0 "NT $R0" 6
   Goto lbl_done

   lbl_winnt_2000:
     Strcpy $R0 '2000'
   Goto lbl_done

   lbl_winnt_XP:
     Strcpy $R0 'XP'
   Goto lbl_done

   lbl_winnt_2003:
     Strcpy $R0 '2003'
   Goto lbl_done

   lbl_winnt_vista:
	 Strcpy $R0 'VISTA'
   Goto lbl_done
   lbl_winnt_7:
	 Strcpy $R0 '7'
   Goto lbl_done

   lbl_error:
     Strcpy $R0 ''
   lbl_done:

   Pop $R1
   Exch $R0
FunctionEnd
!macroend
!insertmacro GetWindowsVersion ""
!insertmacro GetWindowsVersion "un."

###############################
#函数名：GetIEVersion
#基于 Yazno 的函数，http://yazno.tripod.com/powerpimpit/
#返回到堆栈顶
#1-6 (已安装的 IE 版本)或'' (IE 未安装)
#用法:
#Call GetIEVersion
#Pop $R0
#$R0 为 "5" 或其它
Function GetIEVersion
	Push $R0
   	ClearErrors
   	ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"
   	IfErrors lbl_123 lbl_456

lbl_456: ; ie 4+
    Strcpy $R0 $R0 1
   	Goto lbl_done

lbl_123: ; 老版本 IE
    ClearErrors
    ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "IVer"
    IfErrors lbl_error
   	StrCpy $R0 $R0 3
   	StrCmp $R0 '100' lbl_ie1
   	StrCmp $R0 '101' lbl_ie2
   	StrCmp $R0 '102' lbl_ie2

   	StrCpy $R0 '3' ; 如果不是 100、101 或 102 则默认为 IE3
   	Goto lbl_done
lbl_ie1:
    StrCpy $R0 '1'
    Goto lbl_done
lbl_ie2:
    StrCpy $R0 '2'
    Goto lbl_done
lbl_error:
        StrCpy $R0 ''
lbl_done:
    Exch $R0
FunctionEnd


