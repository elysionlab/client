;--------------------------------
!include "MUI.nsh"
!include "WinMessages.nsh"
;plugin
!addplugindir "plugin"

!define INSTALL_NAME				"���Ϸ�"
!define UPDATER_TITLE	   		        "���Ϸ� ���α׷�"
!define INSTALL_FOLDER				"Filerun"
!define REGISTRY_FOLDER				"Filerun"
!define HOMEPAGE_URL				"http://filerun.co.kr"
!define SHORTCUT_NAME_HOMEPAGE			"���Ϸ�"
!define EXENAME_DOWN				"FilerunDown.exe"
!define EXENAME_UP				"FilerunUp.exe"
!define EXEFILE_PATH				"..\Bin"
!define EXPLORER_PATH				"Internet Explorer\iexplore.exe"
!define ICON_FILENAME				"Filerun.ico"
!define CMC_CANDLE_PATH		".\plugin\Candle"
!define URL_PROTOCOL_NAME		"filerun"
!define GRID_PATH			".\plugin\Vgrid"

!define PRODUCT_PUBLISHER "�����ÿ·�"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Filerun.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Filerun"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_DIR_START_REGKEY "Software\Microsoft\Windows\CurrentVersion\Run"
!define PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY "SYSTEM\CurrentControlSet\Services\SharedAccess\Parameters\FirewallPolicy\StandardProfile\AuthorizedApplications\List"
!define PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY "SYSTEM\ControlSet001\Services\SharedAccess\Parameters\FirewallPolicy\StandardProfile\AuthorizedApplications\List"

;--------------------------------
;General

XPStyle on
Name "${UPDATER_TITLE}"
OutFile "Filerun.exe"
;!define MUI_UI "${NSISDIR}\Contrib\UIs\modern2.exe"
;!define MUI_UI_COMPONENTSPAGE_SMALLDESC "${NSISDIR}\Contrib\UIs\modern_smalldesc2.exe"
!define MUI_ICON "${EXEFILE_PATH}\${ICON_FILENAME}"
!define MUI_UNICON "${EXEFILE_PATH}\${ICON_FILENAME}"
InstallDir "$PROGRAMFILES\${INSTALL_FOLDER}"


;--------------------------------
; �������� ����
Function .onInstSuccess
  FindWindow $R0 "" "${UPDATER_TITLE} ��ġ"
  SendMessage $R0 1124 2 0
FunctionEnd

Function UserAbortCall
  FindWindow $R0 "" "${UPDATER_TITLE} ��ġ"
  SendMessage $R0 1124 1 0
  Quit
FunctionEnd


ShowInstDetails Show
BrandingText "${UPDATER_TITLE} ��ġ"

;LicenseBkColor 9BCDDD

;--------------------------------
; �������̽� ����
;!define MUI_HEADERIMAGE
!define MUI_ABORTWARNING
!define MUI_CUSTOMFUNCTION_ABORT "UserAbortCall"
!define MUI_COMPONENTSPAGE_SMALLDESC
;!define MUI_LICENSEPAGE_CHECKBOX

;--------------------------------
; ������ ����
; Welcome page
; License page
;!insertmacro MUI_PAGE_LICENSE "${EXEFILE_PATH}\Filerun_License.txt"

PageEx license
  Caption " "
  LicenseText ""
  LicenseData Filerun_License.txt
  PageCallbacks "" "ExLicense" ""
PageExEnd

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
;LicenseBkColor C5E1EB
;--------------------------------
;Languages
!insertmacro MUI_LANGUAGE "Korean"

;--------------------------------
;Installer Sections
Section "- ${INSTALL_NAME}" SecDummy
;  Call FncAntGuard
  Call FncFileCpy
SectionEnd


;--------------------------------
;�ν��� ����
SectionGroup /e "!���Ϸ� ���α׷�" Main01
  Section "���Ϸ� ���δ�" SEC01
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${EXEFILE_PATH}\FilerunUp.exe"
  SectionEnd

  Section "���Ϸ� �ٿ�δ�" SEC02
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${EXEFILE_PATH}\FilerunDown.exe"
  SectionEnd
SectionGroupEnd

SectionGroup /e "!�ٷΰ��� ������ ����" Main02
  Section "����ȭ�� ������ ����" SEC03
  Call FncDesktop
  SectionEnd
  
  Section "�������� ������ ����" SEC04
  Call FncQuick
  SectionEnd
  
  Section "���۸޴� ������ ����" SEC05
  Call FncStartMenu
  SectionEnd
  
  Section "���ã�⿡ �߰�" SEC06
  Call FncFavorites
  SectionEnd
SectionGroupEnd



Section "-icons" SEC101
SectionIn RO
SetOutPath "$INSTDIR"
SetOverwrite on
  File "${EXEFILE_PATH}\${ICON_FILENAME}"
SectionEnd

  





Section "-DLL ����" SEC200
SectionIn RO
SetOutPath "$INSTDIR"
SetOverwrite on
	File "${CMC_CANDLE_PATH}\Ark32.dll"
	File "${CMC_CANDLE_PATH}\Ark32lgpl.dll"
	File "${CMC_CANDLE_PATH}\Ark32lgplv2.dll"
	File "${CMC_CANDLE_PATH}\Ark32_v2.dll"
	File "${CMC_CANDLE_PATH}\CCheck.dll"
	File "${CMC_CANDLE_PATH}\Check.exe"
	File "${CMC_CANDLE_PATH}\ckpcodec.dll"
	File "${CMC_CANDLE_PATH}\conkeeper.dll"
	File "${CMC_CANDLE_PATH}\conkeeper64.dll"
	File "${CMC_CANDLE_PATH}\detect.exe"
	File "${CMC_CANDLE_PATH}\detect_service.exe"
	File "${CMC_CANDLE_PATH}\MCheck.dll"
	File "${CMC_CANDLE_PATH}\MediaInfo.dll"
	File "${CMC_CANDLE_PATH}\MediaInfo_v2.dll"
	File "${CMC_CANDLE_PATH}\Mfilter.dll"
	File "${CMC_CANDLE_PATH}\mfc100.dll"
	File "${CMC_CANDLE_PATH}\mfc100u.dll"
	File "${CMC_CANDLE_PATH}\msvcp100.dll"
	File "${CMC_CANDLE_PATH}\msvcr100.dll"
	File "${CMC_CANDLE_PATH}\sfdcd.dll"
	File "${CMC_CANDLE_PATH}\sffilerun.dll"
	File "${CMC_CANDLE_PATH}\ssmfileinfo.dll"
	File "${CMC_CANDLE_PATH}\ssmimbc.exe"
	File "${CMC_CANDLE_PATH}\TvScan.dll"
	File "${CMC_CANDLE_PATH}\TvUpdate.dll"
	File "${CMC_CANDLE_PATH}\unace32.exe"
	File "${CMC_CANDLE_PATH}\UNACEV2.DLL"
	File "${CMC_CANDLE_PATH}\ZHashGen.dll"
SectionEnd







;---------------- �׸���----------------------;
  Section "-�׸��� DLL ����" SEC300
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${GRID_PATH}\v_down.dll"
  SectionEnd

  Section "-�׸��� ����" SEC301
  SetOutPath "$INSTDIR"
  SetOverwrite off
	File "${GRID_PATH}\v_filerun_setup.exe"
  Sleep 100
	Exec "$INSTDIR\v_filerun_setup.exe"
  SectionEnd
;----------------------------------------------------------------------;






;Descriptions
;Language strings �� ���Ǳ׷�� ������ ����(����) ���ڿ��� ����
LangString DESC_MAIN01 ${LANG_KOREAN} "���Ϸ� ���� ���� ���α׷� �Դϴ�."
LangString DESC_MAIN02 ${LANG_KOREAN} "�ٷΰ��� �������� ����ϴ�."
LangString DESC_MAIN03 ${LANG_KOREAN} "���� ���α׷� �Դϴ�."
LangString DESC_SEC01 ${LANG_KOREAN} "���Ϸ� ���ε� ���α׷� �Դϴ�."
LangString DESC_SEC02 ${LANG_KOREAN} "���Ϸ� �ٿ�ε� ���α׷� �Դϴ�."
LangString DESC_SEC03 ${LANG_KOREAN} "����ȭ�鿡 ���Ϸ� �ٷΰ��� �������� ����ϴ�."
LangString DESC_SEC04 ${LANG_KOREAN} "�������� â�� ���Ϸ� �ٷΰ��� �������� ����ϴ�."
LangString DESC_SEC05 ${LANG_KOREAN} "���۸޴��� ���Ϸ� �ٷΰ��� �������� ����ϴ�."
LangString DESC_SEC06 ${LANG_KOREAN} "���ã�⿡ ���Ϸ� Ȩ�������� �߰��մϴ�."

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Main01} $(DESC_MAIN01)
    !insertmacro MUI_DESCRIPTION_TEXT ${Sec01} $(DESC_SEC01)
    !insertmacro MUI_DESCRIPTION_TEXT ${Sec02} $(DESC_SEC02)
  !insertmacro MUI_DESCRIPTION_TEXT ${Main02} $(DESC_MAIN02)
    !insertmacro MUI_DESCRIPTION_TEXT ${Sec03} $(DESC_SEC03)
    !insertmacro MUI_DESCRIPTION_TEXT ${Sec04} $(DESC_SEC04)
    !insertmacro MUI_DESCRIPTION_TEXT ${Sec05} $(DESC_SEC05)
    !insertmacro MUI_DESCRIPTION_TEXT ${Sec06} $(DESC_SEC06)
  !insertmacro MUI_DESCRIPTION_TEXT ${Main03} $(DESC_MAIN03)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
Function .onInit
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Mutex_Filerun") i .r1 ?e'
  Pop $R0

  StrCmp $R0 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "��ġ ���α׷��� �̹� �������Դϴ�!"
    Abort
    
  
 ;	InitPluginsDir

	;File /oname=$PLUGINSDIR\splash.bmp "splash.bmp"
	;advsplash::show 1000 600 400 -1 $PLUGINSDIR\splash
	;Pop $0
	;Delete $PLUGINSDIR\splash.bmp

;	Call FncFileCpy
	;Call FncQuick
	;Call FncDesktop
	;Call FncStartMenuTop
;	Call FncInstallEnd
;	Quit

FunctionEnd

;--------------------------------
; ���ν��� ���ϵ�

Section "Uninstall"

  SetDetailsView Show

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  ;��ȭ�� ���� ���α׷����� ����
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunUp.exe"
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunDown.exe"
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunUp.exe"
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunDown.exe"
  ;URL ��������
  DeleteRegKey HKCR "${URL_PROTOCOL_NAME}"

  Delete "$DESKTOP\${SHORTCUT_NAME_HOMEPAGE}.lnk"
  Delete "$QUICKLAUNCH\${SHORTCUT_NAME_HOMEPAGE}.lnk"
  Delete "$STARTMENU\${SHORTCUT_NAME_HOMEPAGE}.lnk"
  Sleep 100

  Delete "$INSTDIR\TachionLive\ENGS\*.*"
  RMDir "$INSTDIR\TachionLive\ENGS"

  Delete "$INSTDIR\TachionLive\*.*"
  RMDir "$INSTDIR\TachionLive"

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

  DeleteRegKey HKCU "Software\${REGISTRY_FOLDER}"

SectionEnd


;done:
;  FileClose $R0
;FunctionEnd

Function ExLicense
  GetDlgItem $MUI_TEMP1 $HWNDPARENT 1037
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:���� ���"
  GetDlgItem $MUI_TEMP1 $HWNDPARENT 1038
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:${INSTALL_NAME} ���α׷�(��)�� ��ġ�Ͻñ� ���� ���� ��� ������ ���캸�ñ� �ٶ��ϴ�."
  FindWindow $R0 "#32770" "" $HWNDPARENT
  ;MessageBox MB_OK $R0
  ;FindWindow $R1 "RichEdit20A" "" $R0
  GetDlgItem $MUI_TEMP1 $R0 1040
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:���� ��� ���� ������ ������ �κ��� ���÷��� [Page Down] Ű�� ���� �ּ���."

  Sleep 100

  Pop $R3
  ;MessageBox MB_OK $R3
  GetDlgItem $MUI_TEMP1 $R0 1001
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:$R3"
FunctionEnd

Function FncFileCpy
  SetAutoClose True
  SetDetailsView Show

  SetOutPath "$INSTDIR"
  SetCompress Auto
  SetOverwrite On

  ;���ν��� ���α׷�
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  ;���α׷� �߰�/���ſ� �߰�
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Filerun.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${INSTALL_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$SYSDIR\${ICON_FILENAME}"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  ;��ȭ�� ���� ���α׷��� �߰�
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunUp.exe" "$INSTDIR\FilerunUp.exe:*:Enabled:FilerunUp"
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunDown.exe" "$INSTDIR\FilerunDown.exe:*:Enabled:FilerunDown"
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunUp.exe" "$INSTDIR\FilerunUp.exe:*:Enabled:FilerunUp"
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunDown.exe" "$INSTDIR\FilerunDown.exe:*:Enabled:FilerunDown"

  ;URL �������� �߰�
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}" "" "URL:${REGISTRY_FOLDER}(Download) Protocol"
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}" "URL Protocol" ""
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\DefaultIcon" "" "$INSTDIR\${EXENAME_DOWN},0"
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\shell" "" ""
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\shell\open" "" ""
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\shell\open\command" "" "$INSTDIR\${EXENAME_DOWN} /url %1"

  SetOutPath "$SYSDIR"
  File "${EXEFILE_PATH}\${ICON_FILENAME}"
  ;File "${EXEFILE_PATH}${SMALLICON_FILENAME}"
FunctionEnd

Function FncDesktop
  CreateShortCut "$DESKTOP\${SHORTCUT_NAME_HOMEPAGE}.lnk" "$PROGRAMFILES\${EXPLORER_PATH}" "${HOMEPAGE_URL}" "$PROGRAMFILES\${INSTALL_FOLDER}\${ICON_FILENAME}" 0 SW_SHOWMAXIMIZED
FunctionEnd

Function FncQuick
  CreateShortCut "$QUICKLAUNCH\${SHORTCUT_NAME_HOMEPAGE}.lnk" "$PROGRAMFILES\${EXPLORER_PATH}" "${HOMEPAGE_URL}" "$PROGRAMFILES\${INSTALL_FOLDER}\${ICON_FILENAME}" 0 SW_SHOWMAXIMIZED
FunctionEnd

Function FncStartMenu
  CreateShortCut "$STARTMENU\${SHORTCUT_NAME_HOMEPAGE}.lnk" "$PROGRAMFILES\${EXPLORER_PATH}" "${HOMEPAGE_URL}" "$PROGRAMFILES\${INSTALL_FOLDER}\${ICON_FILENAME}" 0 SW_SHOWMAXIMIZED
FunctionEnd

Function FncFavorites
  WriteIniStr "$FAVORITES\${SHORTCUT_NAME_HOMEPAGE}.url" "InternetShortcut" "URL" "${HOMEPAGE_URL}"
FunctionEnd