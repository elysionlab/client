;--------------------------------
!include "MUI.nsh"
!include "WinMessages.nsh"
;plugin
!addplugindir "plugin"

!define INSTALL_NAME				"파일런"
!define UPDATER_TITLE				"파일런 프로그램"
!define INSTALL_FOLDER				"Filerun"
!define REGISTRY_FOLDER				"Filerun"
!define HOMEPAGE_URL				"http://filerun.co.kr"
!define SHORTCUT_NAME_HOMEPAGE			"파일런"
!define EXENAME_DOWN				"FilerunDown.exe"
!define EXENAME_UP				"FilerunUp.exe"
!define EXEFILE_PATH				"..\Bin"
!define EXEFILE_PATH_CTRL			"Cab\Bin"
!define EXEFILE_WEBCTRL				"FilerunWebControl.dll"
!define PATH_WEBCTRL_TARGET			"$WINDIR\Downloaded Program Files"
!define ICON_FILENAME				"Filerun.ico"
!define EXPLORER_PATH				"Internet Explorer\iexplore.exe"
!define CMC_CANDLE_PATH		".\plugin\Candle"
!define URL_PROTOCOL_NAME		"filerun"
!define GRID_PATH			".\plugin\Vgrid"

!define PRODUCT_PUBLISHER "엘리시온랩"
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
OutFile "Filerun_setup.exe"
;Icon "${EXEFILE_PATH}${ICON_FILENAME}"
;UninstallIcon "${EXEFILE_PATH}${ICON_FILENAME}"
;!define MUI_UI "${NSISDIR}\Contrib\UIs\modern2.exe"
;!define MUI_UI_COMPONENTSPAGE_SMALLDESC "${NSISDIR}\Contrib\UIs\modern_smalldesc2.exe"
!define MUI_ICON "${EXEFILE_PATH}\${ICON_FILENAME}"
!define MUI_UNICON "${EXEFILE_PATH}\${ICON_FILENAME}"
InstallDir "$PROGRAMFILES\${INSTALL_FOLDER}"
	
;Function UserAbortCall
;  FindWindow $R0 "" "${UPDATER_TITLE}"
;  SendMessage $R0 1124 1 0
;  Quit
;FunctionEnd
;--------------------------------
; 인터페이스 설정
;!define MUI_HEADERIMAGE
!define MUI_ABORTWARNING
;!define MUI_CUSTOMFUNCTION_ABORT "UserAbortCall"
!define MUI_COMPONENTSPAGE_SMALLDESC

;--------------------------------
; 페이지 설정
; Uninstaller pages
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

; Language files
!insertmacro MUI_LANGUAGE "Korean"



;--------------------------------
Function .onInit
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Mutex_Filerun") i .r1 ?e'
  Pop $R0

  StrCmp $R0 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "설치 프로그램이 이미 실행중입니다!"
    Abort
  ;InitPluginsDir
  ;File /oname=$PLUGINSDIR\splash.bmp "splash.bmp"
  ;advsplash::show 1000 600 400 -1 $PLUGINSDIR\splash
  ;Pop $0
  ;Delete $PLUGINSDIR\splash.bmp
  
  
  
  ;인터넷창이 열려있는지 검사
  FindWindow $R0 "IEFrame" ""
  StrCmp $R0 "0" done +1
  MessageBox MB_YESNO|MB_ICONEXCLAMATION "파일런 수동설치를 위해서 열려있는 인터넷창을 모두 닫아야 합니다. 닫으시겠습니까?" IDYES yes
  ;Call FncFileCpy
  ;Call FncQuick
  ;Call FncDesktop
  ;Call FncStartMenu
  MessageBox MB_OK|MB_ICONEXCLAMATION "파일런 프로그램 수동설치를 취소합니다."
  Quit
  
yes:
  FindWindow $R0 "IEFrame" ""
  ;MessageBox MB_OK $R0
  StrCmp $R0 "0" equal notequal
equal:
  Goto done
notequal:
  SendMessage $R0 16 "" ""
  Sleep 300
  Goto yes
  
done:
  ;MessageBox MB_OK "Call FncWdbCtrl"
  Call FncWebCtrl
FunctionEnd

;--------------------------------
Function .onInstSuccess
  MessageBox MB_OK|MB_ICONEXCLAMATION "감사합니다! 수동 설치가 완료 되었습니다!"
  Exec "$PROGRAMFILES\${EXPLORER_PATH} ${HOMEPAGE_URL}"
FunctionEnd

ShowInstDetails Show
BrandingText "${UPDATER_TITLE} 설치"

;--------------------------------
;프로그램 파일
Section "- ${INSTALL_NAME}" SecDummy
;  Call FncAntGuard
  Call FncFileCpy
SectionEnd

;--------------------------------

;인스톨 섹션
SectionGroup /e "!파일런 프로그램" Main01
  Section "파일런 업로더" SEC01
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${EXEFILE_PATH}\FilerunUp.exe"
  SectionEnd

  Section "파일런 다운로더" SEC02
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${EXEFILE_PATH}\FilerunDown.exe"
  SectionEnd
SectionGroupEnd

SectionGroup /e "!바로가기 아이콘 생성" Main02
  Section "바탕화면 아이콘 생성" SEC03
  Call FncDesktop
  SectionEnd

  Section "빠른실행 아이콘 생성" SEC04
  Call FncQuick
  SectionEnd

  Section "시작메뉴 아이콘 생성" SEC05
  Call FncStartMenu
  SectionEnd

  Section "즐겨찾기에 추가" SEC06
  Call FncFavorites
  SectionEnd
SectionGroupEnd




Section "-icons" SEC101
SectionIn RO
SetOutPath "$INSTDIR"
SetOverwrite on
  File "${EXEFILE_PATH}\${ICON_FILENAME}"
SectionEnd








Section "-DLL 파일" SEC200
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










;---------------- 그리드----------------------;
  Section "-그리드 DLL 파일" SEC300
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File "${GRID_PATH}\v_down.dll"
  SectionEnd

  Section "-그리드 어플" SEC301
  SetOutPath "$INSTDIR"
  SetOverwrite off
	File "${GRID_PATH}\v_filerun_setup.exe"
  Sleep 100
	Exec "$INSTDIR\v_filerun_setup.exe"
  SectionEnd
;----------------------------------------------------------------------;






;----------------------------------------
;Descriptions
;Language strings 각 섹션그룹과 섹션의 설명(툴팁) 문자열을 지정
LangString DESC_MAIN01 ${LANG_KOREAN} "파일런 파일 전송 프로그램 입니다."
LangString DESC_MAIN02 ${LANG_KOREAN} "바로가기 아이콘을 만듭니다."
LangString DESC_MAIN03 ${LANG_KOREAN} "제휴 프로그램 입니다."
LangString DESC_SEC01 ${LANG_KOREAN} "파일런 업로드 프로그램 입니다."
LangString DESC_SEC02 ${LANG_KOREAN} "파일런 다운로드 프로그램 입니다."
LangString DESC_SEC03 ${LANG_KOREAN} "바탕화면에 파일런 바로가기 아이콘을 만듭니다."
LangString DESC_SEC04 ${LANG_KOREAN} "빠른실행 창에 파일런 바로가기 아이콘을 만듭니다."
LangString DESC_SEC05 ${LANG_KOREAN} "시작메뉴에 파일런 바로가기 아이콘을 만듭니다."
LangString DESC_SEC06 ${LANG_KOREAN} "즐겨찾기에 파일런 홈페이지를 추가합니다."

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
; 언인스톨 파일들
Section "Uninstall"
  SetDetailsView Show
  
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  ;방화벽 예외 프로그램에서 삭제
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunUp.exe"
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunDown.exe"
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunUp.exe"
  DeleteRegValue HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunDown.exe"
  ;URL 프로토콜
  DeleteRegKey HKCR "${URL_PROTOCOL_NAME}"
  
  Delete "$DESKTOP\${SHORTCUT_NAME_HOMEPAGE}.lnk"
  Delete "$QUICKLAUNCH\${SHORTCUT_NAME_HOMEPAGE}.lnk" 
  Delete "$STARTMENU\${SHORTCUT_NAME_HOMEPAGE}.lnk"
  Sleep 100

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"
  
  DeleteRegKey HKCU "Software\${REGISTRY_FOLDER}"
SectionEnd

;done:
;  FileClose $R0
;FunctionEnd

Function ExLicense
  GetDlgItem $MUI_TEMP1 $HWNDPARENT 1037
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:사용권 계약"
  GetDlgItem $MUI_TEMP1 $HWNDPARENT 1038
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:${INSTALL_NAME} 프로그램(을)를 설치하시기 전에 사용권 계약 내용을 살펴보시기 바랍니다."
  FindWindow $R0 "#32770" "" $HWNDPARENT
  ;MessageBox MB_OK $R0
  ;FindWindow $R1 "RichEdit20A" "" $R0
  GetDlgItem $MUI_TEMP1 $R0 1040
  SendMessage $MUI_TEMP1 ${WM_SETTEXT} 0 "STR:사용권 계약 동의 사항의 나머지 부분을 보시려면 [Page Down] 키를 눌러 주세요."

  Sleep 100

  Delete "$INSTDIR\TachionLive\ENGS\*.*"
  RMDir "$INSTDIR\TachionLive\ENGS"

  Delete "$INSTDIR\TachionLive\*.*"
  RMDir "$INSTDIR\TachionLive"

  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"

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

  File "_ver.ini"
  File "ver.ini"

  ;언인스톨 프로그램 
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  ;프로그램 추가/제거에 추가
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Filerun.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${INSTALL_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$SYSDIR\${ICON_FILENAME}"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  ;WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  ;방화벽 예외 프로그램에 추가
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunUp.exe" "$INSTDIR\FilerunUp.exe:*:Enabled:FilerunUp"
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION1_REGKEY}" "$INSTDIR\FilerunDown.exe" "$INSTDIR\FilerunDown.exe:*:Enabled:FilerunDown"
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunUp.exe" "$INSTDIR\FilerunUp.exe:*:Enabled:FilerunUp"
  WriteRegStr HKLM "${PROCUCT_DIR_FIREWALL_EXCEPTION2_REGKEY}" "$INSTDIR\FilerunDown.exe" "$INSTDIR\FilerunDown.exe:*:Enabled:FilerunDown"

  ;URL 프로토콜 추가
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}" "" "URL:${REGISTRY_FOLDER}(Download) Protocol"
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}" "URL Protocol" ""
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\DefaultIcon" "" "$INSTDIR\${EXENAME_DOWN},0"
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\shell" "" ""
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\shell\open" "" ""
  WriteRegStr HKCR "${URL_PROTOCOL_NAME}\shell\open\command" "" "$INSTDIR\${EXENAME_DOWN} /url %1"

  SetOutPath "$SYSDIR"
  File "${EXEFILE_PATH}\${ICON_FILENAME}"
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

Function FncWebCtrl
  SetAutoClose True
  SetOutPath "${PATH_WEBCTRL_TARGET}"
  SetCompress Auto
  SetOverwrite On

;  File "${EXEFILE_PATH_CTRL}\${EXEFILE_WEBCTRL}"
  File "${EXEFILE_PATH}\${EXEFILE_WEBCTRL}"
  RegDLL "${PATH_WEBCTRL_TARGET}\${EXEFILE_WEBCTRL}"
FunctionEnd
