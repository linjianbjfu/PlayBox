;���ú���ͷ�ļ�
!include "FileFunc.nsh"
!include LogicLib.nsh
!include UsefulLib.nsh
!insertmacro GetSize

var param
var loc_var
###############################
#��������JudgeUpdate
#���������$R0:��װ·��
#Ч�������������$R1����1
#       ����$R1����0
Function JudgeUpdate
	System::Call 'kernel32::GetModuleFileNameA(i 0, t .R2, i 1024) i r1'
  	${StrStrI} $0 "$R2" "$R0"
 	${If} $0 == ""
		StrCpy $R1 "0"
  	${Else}
		;�Ƿ��Ѿ���װ����
		IfFileExists "$R0\Tool.exe" hasInstalled notInstalled
notInstalled:
      	StrCpy $R1 "0"
      	Goto installedEnd
hasInstalled:
	  	;����װĿ¼��¼����
      	StrCpy $R1 "1"
	  	;���������֣���Ĭ�ͳ�������
		${StrStrI} $0 $R0 "Silent"
		${If} $0 == "" ;��������
	      	SetSilent normal
		${Else} ;��Ĭ����
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
#��������RenameAndDeleteReboot
#���������$R0��ΪҪ�������ļ�����·��
#Ч������$R0�е��ļ����޸�Ϊ$R0.0.tmp��
#	   ���$R0.0.tmp���������Ϊ$R0.1.tmp
#      ���$R0.1.tmp���������Ϊ$R0.2.tmp
#	   �մ˷�������
#      ���ļ���������$R1.
Function RenameFile
	;$R0ԭʼ�ļ���
	IntOp $R2 0 + 0 ;$R2����
	StrCpy $R1 "$R0.$R2.tmp" ;$R1�޸ĺ���ļ���
	IfFileExists $R1 exist notExist
exist:
	IntOp $R2 $R2 + 1
	StrCpy $R1 "$R0.$R2.tmp"
	IfFileExists $R1 exist notExist
notExist:
	Rename $R0 $R1
FunctionEnd


###############################
#��������RenameAndDeleteReboot
#���������$R0��ΪҪ�������ļ�����·��
#Ч������$R0�е��ļ����޸�Ϊ$R0.0.tmp��
#	   ���$R0.0.tmp���������Ϊ$R0.1.tmp
#      ���$R0.1.tmp���������Ϊ$R0.2.tmp
#	   �մ˷�������
Function RenameAndDeleteReboot
	;$R0ԭʼ�ļ���
	IntOp $R2 0 + 0 ;$R2����
	StrCpy $R1 "$R0.$R2.tmp" ;$R1�޸ĺ���ļ���
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
#��������RenameAndDeleteReboot
#���������$R0��ΪҪ�������ļ�����·��
#Ч������$R0�е��ļ����޸�Ϊ$R0.0.tmp��
#	   ���$R0.0.tmp���������Ϊ$R0.1.tmp
#      ���$R0.1.tmp���������Ϊ$R0.2.tmp
#	   �մ˷�������
Function un.RenameAndDeleteReboot
	;$R0ԭʼ�ļ���
	IntOp $R2 0 + 0 ;$R2����
	StrCpy $R1 "$R0.$R2.tmp" ;$R1�޸ĺ���ļ���
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
#��������GetWindowsVersion
# ���ص���ջ��
#
# Windows �汾 (95, 98, ME, NT x.x, 2000, XP, 2003)
# ��
# '' (δ֪ Windows �汾)
#
# �÷�:
#   Call GetWindowsVersion
#   Pop $R0
# �ú�����Դ�ڰ����ĵ�
!macro GetWindowsVersion un
Function ${un}GetWindowsVersion
   Push $R0
   Push $R1
   ClearErrors
   ReadRegStr $R0 HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
   IfErrors 0 lbl_winnt
   ; ���� NT
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
#��������GetIEVersion
#���� Yazno �ĺ�����http://yazno.tripod.com/powerpimpit/
#���ص���ջ��
#1-6 (�Ѱ�װ�� IE �汾)��'' (IE δ��װ)
#�÷�:
#Call GetIEVersion
#Pop $R0
#$R0 Ϊ "5" ������
Function GetIEVersion
	Push $R0
   	ClearErrors
   	ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"
   	IfErrors lbl_123 lbl_456

lbl_456: ; ie 4+
    Strcpy $R0 $R0 1
   	Goto lbl_done

lbl_123: ; �ϰ汾 IE
    ClearErrors
    ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "IVer"
    IfErrors lbl_error
   	StrCpy $R0 $R0 3
   	StrCmp $R0 '100' lbl_ie1
   	StrCmp $R0 '101' lbl_ie2
   	StrCmp $R0 '102' lbl_ie2

   	StrCpy $R0 '3' ; ������� 100��101 �� 102 ��Ĭ��Ϊ IE3
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


