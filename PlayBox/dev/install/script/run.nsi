Name "小宝贝游戏盒"
OutFile "..\install_resource\run.exe"

SetCompressor /SOLID /FINAL lzma
Caption "小宝贝游戏盒"
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
  ;Exec "$R0\PlayBox.exe startuprun"  ;参数startuprun，第一次运行游戏盒时，会最小化
  Exec "$R0\PlayBox.exe"
  Return

NotFound:

SectionEnd


