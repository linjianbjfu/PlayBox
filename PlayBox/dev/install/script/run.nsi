Name "С������Ϸ��"
OutFile "..\install_resource\run.exe"

SetCompressor /SOLID /FINAL lzma
Caption "С������Ϸ��"
Icon ..\..\prj\PlayBox\res\xiong.ico
AutoCloseWindow true

Function .onInit
    SetSilent silent
FunctionEnd

Section "run"
  ClearErrors
  StrCpy $R0 ""
  ReadRegStr $R0 HKLM "SOFTWARE\YPLAYBOX" "path"
  StrCmp $R0 "" NotFound 0
  IfFileExists "$R0\PlayBox.exe" 0 NotFound
  ;Exec "$R0\PlayBox.exe startuprun"  ;����startuprun����һ��������Ϸ��ʱ������С��
  Exec "$R0\PlayBox.exe"
  Return

NotFound:

SectionEnd


