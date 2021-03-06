; MUI 1.67 compatible ------
RequestExecutionLevel admin
;===============包含头文件===============
;!include "MUI.nsh"
!include "FileFunc.nsh"
!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
!include "Memento.nsh"
!include "WordFunc.nsh"
!include "nsDialogs_createIPaddress.nsh"
;===============宏定义 宏名===============
!define PRODUCT_NAME "中铁工程建设知识库客户端"
!define PRODUCT_VERSION "20120524"
!define PRODUCT_PUBLISHER "安世亚太"
!define PRODUCT_WEB_SITE "http://www.peraglobal.com"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\中铁网盘客户端"
!define PERAGLOBAL_FLEXWARE_KEY "Software\Peraglobal"
!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\中铁网盘客户端"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_CURRENTUSER__KEY HKCU
!define PRODUCT_CLASSES_R_KEY HKCR
!define PRODUCT_JRE_RUNTIME_KEY "SOFTWARE\JavaSoft\Java Runtime Environment\"
!define PRODUCT_JRE_RUN_TIME_PATH "\lib\ext"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
!define MUI_HEADERIMAGE ;更改lisence页面标题图片
!define ENV_REG_KEY "\SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
!define Src ".\install" ;源文件根目录
!define COMPONENT "PeraComponent"
!define CLIENT "Client"
!define DEPENDS "Depends"
!define PERAWORKSPACE "PeraWorkSpace"
!define PERADESKCLIENT "PeraDeskClient"
!define /date DATE "%Y%m%d"
!define SHCNE_ASSOCCHANGED 0x8000000
!define SHCNF_IDLIST 0
;===============自定义 变量===============
Var StartMenuFolder
Var IP
Var strIP
Var Port
Var strPort
!include nsDialogs.nsh
Var FilePathDir
Var SetupProjcetFlag
Var RADIO_REPAIR
Var RADIO_REMOVE
Var Checkbox_State_REPAIR
Var Checkbox_State_REMOVE
Var Checkbox_State

Var DependsStatus
Var	PeraDeskClientStatus
Var PeraComponentStatus
Var PeraWorkSpaceStatus
Var UninstallPath
;===============宏定义 界面===============
; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON ".\Assist\InstallerRes\PeraWs.ico"
!define MUI_UNICON ".\Assist\InstallerRes\unin.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP ".\Assist\InstallerRes\newleft.bmp"
!define MUI_HEADERIMAGE_BITMAP_NOSTRETCH
!define MUI_HEADERIMAGE_BITMAP ".\Assist\InstallerRes\head.bmp"
; Welcome page
;!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_HEADER_TRANSPARENT_TEXT
Page custom nsDialogsRepair nsDialogsRepairLeave
;!insertmacro MUI_PAGE_LICENSE ".\Assist\Authorize\软件用户许可安装协议.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
; 开始菜单设置页面
!define MUI_STARTMENUPAGE_TEXT_CHECKBOX "不要创建开始菜单"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
;!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
;Page custom CONFIG_PAGE LEAVE_CONFIG_PAGE
!define MUI_COMPONENTSPAGE_TEXT_TOP "以下列出了安装程序将要安装的组件，单击[安装]继续。"
!define MUI_COMPONENTSPAGE_TEXT_COMPLIST  "客户端组件列表："
!define MUI_PAGE_CUSTOMFUNCTION_PRE CompShowProc
;!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH
; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES
; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"
; MUI end ------

BrandingText "${PRODUCT_PUBLISHER}"
Name "${PRODUCT_NAME}"
OutFile "..\Output\${PRODUCT_NAME}安装包(${DATE}).exe"
InstallDir "C:\WorkSpace"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "JWBClient") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_OK|MB_ICONEXCLAMATION "安装程序已经在运行。"
	Abort
	KillProcDLL::KillProc
	InitPluginsDir
	File /oname=$PLUGINSDIR\splash.bmp ".\Assist\InstallerRes\splash.bmp" ;启动时的splash图片
	;启动时的闪屏
	;advsplash::show 1500 800 600 -1 $PLUGINSDIR\splash
	;禁止重复安装程序
	ReadRegStr $0 HKLM '${PRODUCT_UNINST_KEY}' "DisplayName"
	ReadRegStr $SetupProjcetFlag HKLM "${PERAGLOBAL_FLEXWARE_KEY}" "SetupProjcetFlag"
	ReadRegStr $FilePathDir HKLM "${PERAGLOBAL_FLEXWARE_KEY}" "RootDir"


   ${if} $SetupProjcetFlag == "PERA精益研发平台-综合设计客户端"
   	MessageBox MB_OK|MB_ICONEXCLAMATION "本机已安装综合设计客户端，导致系统冲突，请卸载。"
   	Quit
   ${EndIf}
    ${if} $FilePathDir != ""
    	MessageBox MB_OK|MB_ICONEXCLAMATION "本机已安装综合设计客户端，导致系统冲突，请卸载。"
    	Quit
    ${EndIf}
	!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;==========================================================基础定义函数结束==================================================================
;==========================================================组件列表=============================================================================
Section "主程序" ZCX
SectionIn RO
SetShellVarContext all
	WriteRegStr   HKLM "${PERAGLOBAL_FLEXWARE_KEY}" "SetupProjcetFlag" "中铁网盘中间件组件"
	;===============Copy文件到COMPONENT目录 规避了.svn文件===============


	;===============检测Framework4.0 没安装就释放文件执行安装===============
	SetOutPath "$INSTDIR\PeraNetDisk"
	File /r ".\Assist\Plugin\msxml.msi"
	Execwait 'msiexec  /qn /i "$INSTDIR\PeraNetDisk\msxml.msi"'
	ReadRegStr $DependsStatus HKLM "${PERAGLOBAL_FLEXWARE_KEY}" "PeraNetDisk"
;===================================协议拷贝================================================================
	 SetOutPath "$INSTDIR\PeraNetDisk"
	 File /r /x *.svn "..\PeraNetDiskOutPut\"
   SetOutPath "$INSTDIR\PeraNetDisk"
   File /r ".\Assist\Authorize\软件用户许可安装协议.txt"
;====================================安装时注册COM，写自启动注册表======================================================
	RegDLL "$INSTDIR\PeraNetDisk\PeraClientConfig.dll"
	RegDLL "$INSTDIR\PeraNetDisk\PeraNetDiskCom.dll"
	
		CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\卸载中铁网盘客户端.LNK" "$INSTDIR\PeraNetDisk\卸载中铁网盘客户端.exe"
;==============================================================================================================
;==========================================================向Config.ini文件中写入配置信息=======================================

	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk CacheDir $INSTDIR\CacheDir
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk UploadMaxTask 5
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk DownloadMaxTask 5
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk UploadSpeedLimit 0
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk DownloadSpeedLimit 0
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk DownloadConnectTimeout 10000
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk UploadUrl  "http://cnki.crec.cn/dms/netDiskRPC"
	WriteINIStr $INSTDIR\PeraNetDisk\Config.ini PeraDCNetDisk DownloadUrl  "http://cnki.crec.cn/dms/netDisk/down?id="
SectionEnd


;=============================================控件树 区段组件描述=================================================================
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;=======================================================函数封装区域==============================================================
Function KillDll

FunctionEnd
;=======================================================IP窗口封装函数（创建执行）================================================
Function CONFIG_PAGE
	nsDialogs::Create 1018
	Pop $0
	${If} $0 == error
	Abort
	${EndIf}
	${NSD_CreateLabel} 6u 10u 200u 36u  "请配置服务器地址$\r$\n$\r$\n请确认无误后点击[安装(I)]按钮"
	${NSD_CreateGroupBox} 0u 80u 100% 55u  "请输入服务器地址"
	Pop $0
	${NSD_InitIPaddress}
	${NSD_CreateIPaddress} 20u 103u 100u 15u $strIP
	Pop $IP
	SendMessage $IP ${WM_SETTEXT} 0 "STR:127.0.0.1"
	${NSD_CreateLabel} 122u 99u 5u 15u  " ："
	${NSD_CreateNumber} 130u 103u 30u 15u $strPort
	Pop $Port
	SendMessage $Port ${WM_SETTEXT} 0 "STR:8080"

	nsDialogs::Show
FunctionEnd
;=======================================================IP窗口封装函数（退出执行）=================================================
Function LEAVE_CONFIG_PAGE
  	${NSD_GetText} $IP $strIP
  	${If} $strIP == "0.0.0.0"
		MessageBox MB_OK "请输入服务器IP地址，谢谢合作！"
		Abort
	${EndIf}
	${NSD_GetText} $Port $strPort
  	${If} $strPort == ""
		MessageBox MB_OK "请输入服务器端口号，谢谢合作！"
		Abort
	${EndIf}
FunctionEnd
;=======================================================IP窗口封装函数结束========================================================
;=======================================================修复卸载窗口==============================================================
Function nsDialogsRepairLeave
  ${NSD_GetState} $RADIO_REPAIR $Checkbox_State_REPAIR
  ${NSD_GetState} $RADIO_REMOVE $Checkbox_State_REMOVE
  ${If} $Checkbox_State_REMOVE == ${BST_CHECKED}
  
  ReadRegStr $UninstallPath ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  Exec $UninstallPath
    Quit
  ${EndIf}
  StrCpy $INSTDIR $FilePathDir

  SendMessage $HWNDPARENT 0x408 5 0
FunctionEnd

Function nsDialogsRepair
  ${if} $SetupProjcetFlag == ""
  	${if} $FilePathDir == ""
    	Abort
    ${EndIf}
  ${EndIf}
 ${if} $FilePathDir == ""
    	Abort
    ${EndIf}
 nsDialogs::Create /NOUNLOAD 1018
 ${NSD_CreateLabel} 10u 0u 300u 30u "PERA精益研发平台-综合设计客户端已经安装，请选择您要执行的操作，并点击『下一步(N)』继续"
 ${if} $SetupProjcetFlag == "中铁网盘中间件组件"
   ${NSD_CreateLabel} 10u 0u 300u 30u "中铁网盘中间件已经安装，请选择您要执行的操作，并点击『下一步(N)』继续"
  ${EndIf}
  ${if} $SetupProjcetFlag == "PERA精益研发平台-综合设计客户端"
   ${NSD_CreateLabel} 10u 0u 300u 30u "PERA精益研发平台-综合设计客户端已经安装，请选择您要执行的操作，并点击『下一步(N)』继续"
  ${EndIf}

  ${NSD_CreateRadioButton}  40u 60u 100u 30u "卸载"
  Pop $RADIO_REMOVE
 ${If} $Checkbox_State_REMOVE == ${BST_CHECKED}
  ${NSD_Check} $RADIO_REMOVE
  ${NSD_GetState} $RADIO_REMOVE $Checkbox_State
 ${EndIf}

  ${If} $Checkbox_State <> ${BST_CHECKED}
    ${NSD_Check} $RADIO_REPAIR
  ${EndIf}
 nsDialogs::Show
FunctionEnd
;=======================================================修复卸载窗口结束===========================================================
;=======================================================组件选择窗口===========================================================
Function CompShowProc

  ${if} $FilePathDir != ""
		${If} $PeraComponentStatus != ""
			IntOp $0 ${SF_SELECTED} | ${SF_RO}
			SectionSetFlags ${ZJFZHJ} $0	
		${EndIf}
		${If} $PeraWorkSpaceStatus != ""
			IntOp $0 ${SF_SELECTED} | ${SF_RO}
			SectionSetFlags ${ZHSJHJ} $0

		${EndIf}
	  ${If} $PeraDeskClientStatus != ""
	  	IntOp $0 ${SF_SELECTED} | ${SF_RO}
			SectionSetFlags ${GRGZPT} $0

		${EndIf}
  ${EndIf}
   GetDlgItem $1 $HWNDPARENT 3 ;上一步
  ShowWindow $1 ${SW_HIDE}
FunctionEnd
;=======================================================组件选择窗口结束===========================================================

;=======================================================开始菜单快捷方式===========================================================
Section -AdditionalIcons
SectionEnd
;=======================================================开始菜单快捷方式结束========================================================
Section -Post
	WriteUninstaller "$INSTDIR\PeraNetDisk\卸载中铁网盘客户端.exe"
	WriteRegStr  HKLM "${PERAGLOBAL_FLEXWARE_KEY}" "SetupProjcetFlag" "中铁网盘中间件组件"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\PeraNetDisk\卸载中铁网盘客户端.exe"
;===============刷新桌面===============

 	rfshdktp::refreshDesktop


SectionEnd
Function .onInstSuccess

FunctionEnd
Function un.onUninstSuccess
	MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
	SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE
	FindProcDLL::FindProc "OpenTools.exe"
	StrCmp $R0 1 +3 +1
	FindProcDLL::FindProc "Flexware.Runtime.GRCRunners.exe"
	StrCmp $R0 1 +1 +4
	MessageBox MB_ICONINFORMATION|MB_RETRYCANCEL "安装程序检测到$(^Name)正在运行！请退出程序重试或取消本次安装！"   IDRetry Retry IDCANCEL Cancel
Retry:
	Goto -5
Cancel:
	Quit
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "确定要完全移除$(^Name)及所有组件？" IDYES +2
	Abort
FunctionEnd
;==============================组件列表选择回调函数=========================================


;=======================================================================
Section Uninstall
;===============停止进程并移除服务===============
	KillProcDLL::KillProc "PeraDCNetDiskDaemon.exe"

	DeleteRegKey HKLM "${PERAGLOBAL_FLEXWARE_KEY}"
;=================反注册PeraFileUpDown.dll=======================================================
        
  UnRegDLL "$INSTDIR\PeraNetDisk\PeraClientConfig.dll"
	UnRegDLL "$INSTDIR\PeraNetDisk\PeraNetDiskCom.dll"

;===============删除目录文件===============

;===============删除快捷方式===============
SetShellVarContext all
	Delete "$DESKTOP\中铁网盘客户端.LNK"
	
RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

;===============客户端卸载处理===============
;===============协同工作台卸载处理===============

;===============删除最外层目录===============
        Delete "$INSTDIR\*.*"
	RMDir /r /REBOOTOK "$INSTDIR"
;===============刷新桌面===============
	rfshdktp::refreshDesktop
	SetAutoClose true
SectionEnd