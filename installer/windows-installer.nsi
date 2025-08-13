!include "MUI2.nsh"

!define PLUGIN_NAME "FoxClip"
!define OBS_SUBDIR "obs-studio"
!define OBS_PLUGINS_SUBDIR "obs-plugins"
!define OBS_PLUGINS_BIN_SUBDIR "64bit"
!define OBS_PLUGINS_DATA_SUBDIR "data\obs-plugins"
; 既定 InstallDir は後で動的に上書き
InstallDir "$PROGRAMFILES64\${OBS_SUBDIR}"
SetRegView 64
RequestExecutionLevel admin
Var OBSPath

Function .onInit
  StrCpy $OBSPath ""
  ; 1) レジストリ (通常版インストーラ)
  ReadRegStr $OBSPath HKLM "Software\OBS Studio" "Path"
  ; 2) Microsoft Uninstall キー (将来互換)
  ${If} $OBSPath == ""
    ReadRegStr $OBSPath HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OBS Studio" "InstallLocation"
  ${EndIf}
  ; 3) 既定パス候補
  ${If} $OBSPath == ""
    StrCpy $OBSPath "$PROGRAMFILES64\obs-studio"
  ${EndIf}
  ; obs64.exe が存在するか確認
  ${IfNot} ${FileExists} "$OBSPath\bin\64bit\obs64.exe"
    MessageBox MB_ICONEXCLAMATION "OBS のインストール場所を検出できません。次で手動選択してください。"
    StrCpy $INSTDIR "$PROGRAMFILES64\obs-studio"
  ${Else}
    StrCpy $INSTDIR "$OBSPath"
  ${EndIf}
FunctionEnd

Name "${PRODUCT_NAME}"
OutFile "${OUTPUT_FILE}"
InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}"
RequestExecutionLevel admin

Page Directory
Page InstFiles

Section "Install"
  SetOutPath "$INSTDIR"
  File /r "${SOURCE_DIR}\*.*"
SectionEnd

Section "Plugin Files"
  SetOutPath "$INSTDIR\${OBS_PLUGINS_SUBDIR}\${OBS_PLUGINS_BIN_SUBDIR}"
  File /oname=${PLUGIN_NAME}.dll "build\Release\${PLUGIN_NAME}.dll"
  ; PDB (任意)
  ${If} ${FileExists} "build\Release\${PLUGIN_NAME}.pdb"
    File "build\Release\${PLUGIN_NAME}.pdb"
  ${EndIf}
SectionEnd

Section "Plugin Data"
  SetOutPath "$INSTDIR\${OBS_PLUGINS_DATA_SUBDIR}\${PLUGIN_NAME}"
  File /r "data\*.*"
SectionEnd

Section -Post
  ; アンインストール情報
  WriteRegStr HKLM "Software\${PLUGIN_NAME}" "InstallPath" "$INSTDIR"
  WriteUninstaller "$INSTDIR\${OBS_PLUGINS_SUBDIR}\${OBS_PLUGINS_BIN_SUBDIR}\Uninstall_${PLUGIN_NAME}.exe"
SectionEnd

Section "Uninstall"
  ReadRegStr $OBSPath HKLM "Software\${PLUGIN_NAME}" "InstallPath"
  ${If} $OBSPath == ""
    StrCpy $OBSPath "$INSTDIR"
  ${EndIf}
  Delete "$OBSPath\${OBS_PLUGINS_SUBDIR}\${OBS_PLUGINS_BIN_SUBDIR}\${PLUGIN_NAME}.dll"
  Delete "$OBSPath\${OBS_PLUGINS_SUBDIR}\${OBS_PLUGINS_BIN_SUBDIR}\${PLUGIN_NAME}.pdb"
  RMDir /r "$OBSPath\${OBS_PLUGINS_DATA_SUBDIR}\${PLUGIN_NAME}"
  Delete "$OBSPath\${OBS_PLUGINS_SUBDIR}\${OBS_PLUGINS_BIN_SUBDIR}\Uninstall_${PLUGIN_NAME}.exe"
  DeleteRegKey HKLM "Software\${PLUGIN_NAME}"
SectionEnd
