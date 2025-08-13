!include "MUI2.nsh"
!include "x64.nsh"

!ifndef PRODUCT_NAME
  !define PRODUCT_NAME "FoxClip"
!endif
!ifndef PRODUCT_VERSION
  !define PRODUCT_VERSION "0.0.0"
!endif
!ifndef SOURCE_DIR
  !define SOURCE_DIR "release\RelWithDebInfo"
!endif
!ifndef OUTPUT_FILE
  !define OUTPUT_FILE "installer.exe"
!endif
!ifndef INPUT_DIR
  !define INPUT_DIR "release"
!endif

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${OUTPUT_FILE}"

InstallDir "$PROGRAMFILES64\obs-studio"   ; ← ここだけで指定 (重複禁止)
InstallDirRegKey HKLM "Software\OBS Studio" "InstallPath"

Var OBSPath

!define OBS_BIN_SUBDIR   "obs-plugins\64bit"
!define OBS_DATA_SUBDIR  "data\obs-plugins"

Function .onInit
  ; レジストリビュー選択
  ${If} ${RunningX64}
    SetRegView 64
  ${Else}
    SetRegView 32
  ${EndIf}

  StrCpy $OBSPath ""
  ReadRegStr $OBSPath HKLM "Software\OBS Studio" "Path"
  ${If} $OBSPath == ""
    ReadRegStr $OBSPath HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OBS Studio" "InstallLocation"
  ${EndIf}
  ${If} $OBSPath == ""
    StrCpy $OBSPath "$PROGRAMFILES64\obs-studio"
  ${EndIf}

  ; 確認
  ${IfNot} ${FileExists} "$OBSPath\bin\64bit\obs64.exe"
    MessageBox MB_ICONEXCLAMATION "OBS のインストール場所が検出できません。次で手動選択してください。"
    StrCpy $INSTDIR "$PROGRAMFILES64\obs-studio"
  ${Else}
    StrCpy $INSTDIR "$OBSPath"
  ${EndIf}
FunctionEnd

InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}"
RequestExecutionLevel admin

Page Directory
Page InstFiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
  SetOutPath "$INSTDIR\obs-plugins\64bit"
  File "${INPUT_DIR}\${PRODUCT_NAME}\bin\64bit\${PRODUCT_NAME}.dll"
  SetOutPath "$INSTDIR\data\obs-plugins\${PRODUCT_NAME}"
  File /r "${INPUT_DIR}\${PRODUCT_NAME}\data\*.*"
  WriteUninstaller "$INSTDIR\Uninstall-${PRODUCT_NAME}.exe"
SectionEnd

Section "Plugin Files"
  SetOutPath "$INSTDIR\${OBS_BIN_SUBDIR}"
  ; DLL (必須)
  File "/oname=${PRODUCT_NAME}.dll" "${SOURCE_DIR}\${PRODUCT_NAME}.dll"
  ; PDB (任意)
  File /nonfatal "${SOURCE_DIR}\${PRODUCT_NAME}.pdb"
SectionEnd

Section "Plugin Data"
  SetOutPath "$INSTDIR\${OBS_DATA_SUBDIR}\${PRODUCT_NAME}"
  File /r "data\*.*"
SectionEnd

Section -Post
  WriteRegStr HKLM "Software\${PRODUCT_NAME}" "InstallPath" "$INSTDIR"
  WriteUninstaller "$INSTDIR\${OBS_BIN_SUBDIR}\Uninstall_${PRODUCT_NAME}.exe"
SectionEnd

Section "Uninstall"
  ReadRegStr $OBSPath HKLM "Software\${PRODUCT_NAME}" "InstallPath"
  ${If} $OBSPath == ""
    StrCpy $OBSPath "$INSTDIR"
  ${EndIf}
  Delete "$OBSPath\${OBS_BIN_SUBDIR}\${PRODUCT_NAME}.dll"
  Delete "$OBSPath\${OBS_BIN_SUBDIR}\${PRODUCT_NAME}.pdb"
  RMDir /r "$OBSPath\${OBS_DATA_SUBDIR}\${PRODUCT_NAME}"
  Delete "$OBSPath\${OBS_BIN_SUBDIR}\Uninstall_${PRODUCT_NAME}.exe"
  DeleteRegKey HKLM "Software\${PRODUCT_NAME}"
SectionEnd
