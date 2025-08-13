!include "MUI2.nsh"

!ifndef PRODUCT_NAME
  !define PRODUCT_NAME "FoxClip"
!endif
!ifndef PRODUCT_VERSION
  !define PRODUCT_VERSION "0.0.0"
!endif
!ifndef OUTPUT_FILE
  !define OUTPUT_FILE "installer.exe"
!endif
!ifndef SOURCE_DIR
  !define SOURCE_DIR "release\RelWithDebInfo"
!endif

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${OUTPUT_FILE}"

; Only one InstallDir (warning 6029 回避)
InstallDir "$PROGRAMFILES64\obs-studio"
InstallDirRegKey HKLM "Software\OBS Studio" "InstallPath"

RequestExecutionLevel admin

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
  ; Plugin binary
  SetOutPath "$INSTDIR\obs-plugins\64bit"
  File "${SOURCE_DIR}\${PRODUCT_NAME}.dll"
  File /nonfatal "${SOURCE_DIR}\${PRODUCT_NAME}.pdb"

  ; Plugin data (リポジトリ直下 data/ から同梱)
  SetOutPath "$INSTDIR\data\obs-plugins\${PRODUCT_NAME}"
  File /r "data\*.*"

  WriteUninstaller "$INSTDIR\Uninstall-${PRODUCT_NAME}.exe"
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\obs-plugins\64bit\${PRODUCT_NAME}.dll"
  Delete "$INSTDIR\obs-plugins\64bit\${PRODUCT_NAME}.pdb"
  RMDir /r "$INSTDIR\data\obs-plugins\${PRODUCT_NAME}"
  Delete "$INSTDIR\Uninstall-${PRODUCT_NAME}.exe"
SectionEnd
