;#RequireAdmin
#Region
#AutoIt3Wrapper_UseUpx=N
#EndRegion

#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.8.1
 Author:         myName

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------

; Script Start - Add your code below here
#include <File.au3>
#include <Array.au3>
#include <WindowsConstants.au3>
#include <GUIConstantsEx.au3>
#include <StaticConstants.au3>
#include <GUIConstantsEx.au3>
#include <Process.au3>

Global $status, $statusvideo
Global $VERSION = "4"

Func installdone()
   $hui = GUICreate("GarageFarm.NET Installer", 300, 130)
   AutoItSetOption("GUIOnEventMode", 1)

   GUISetIcon(@TempDir & "\uploader.ico", 0)
   GUISetBkColor(0xffffff)

   $lab2=GUICtrlCreateLabel("Installation complete!", 10, 30,290,40,1)

   $BTN = GUICtrlCreateLabel("Got it, start renderBeamer", 50, 70, 200,25, BitOr($SS_NOTIFY, $SS_CENTER, $SS_CENTERIMAGE))

   GUICtrlSetFont($lab2,16,Default,Default,"Arial") ;zmiana czcionki
   GUICtrlSetColor($lab2,0x67A9D9)
   GUICtrlSetColor($BTN,0xffffff)
   GUICtrlSetBkColor($BTN,0x67A9D9)
   GUICtrlSetOnEvent($BTN, "openBeamer")
   GUISetOnEvent($GUI_EVENT_CLOSE, "closePopup")
   GUICtrlSetCursor($BTN,0)

   GUISetState(@SW_SHOW, $hui)

   While 1
	  sleep(1)
   WEnd

EndFunc


Func OKButton()
   Global $status = True
EndFunc

Func closePopup()
   if $statusvideo=$GUI_CHECKED Then
	   ShellExecute("https://www.youtube.com/watch?v=tcOmz04cKuA&index=2&list=PLBGFQYyaa0jQ0pI39nTOGcfZXNMxuEhuT")
   EndIf

   Exit
   GUIDelete(@GUI_WinHandle)
   Exit
EndFunc

Func openBeamer()
   Run(@UserProfileDir&"\renderbeamer\jre8\bin\renderbeamer.exe -jar "&'"'&@UserProfileDir&"\renderbeamer\uploader.jar"&'"')
	if $statusvideo=$GUI_CHECKED Then
		ShellExecute("https://www.youtube.com/watch?v=tcOmz04cKuA&index=2&list=PLBGFQYyaa0jQ0pI39nTOGcfZXNMxuEhuT")
	EndIf
	Exit
	GUIDelete(@GUI_WinHandle)
	Exit
EndFunc

Func Terms()
   ShellExecute("https://garage.farm/terms.html")

EndFunc

Func Terms2()
   Global $status = False

EndFunc

Func Guides()
   ShellExecute("http://garagefarm.net/index.php/guides?lang=en")
EndFunc

Func Forum()
   ShellExecute("https://garage.farm/forum/")

EndFunc

Func installstart()
   ;StyleToggle(1)
   Global $status = False
   $hui = GUICreate("GarageFarm.NET Installer", 540, 220)
   GUISetIcon(@TempDir & "\uploader.ico", 0)
   GUISetBkColor(0xffffff)

   $lab=GUICtrlCreateLabel("Welcome to GarageFarm.NET Installer Wizard", 0, 0,540,50,BitOR($SS_CENTER, $SS_CENTERIMAGE))
   $lab2=GUICtrlCreateLabel("renderbeamer will be installed on your PC", 0, 70,540,50,$SS_CENTER)
   ;$lab3= GUICtrlCreateLabel("Click Install to continue...", 5, 100,430,20,$SS_LEFT)
   $lab4= GUICtrlCreateLabel("Terms of Service", 320, 110,170,20,BitOr($SS_NOTIFY, $SS_CENTER, $SS_CENTERIMAGE))

   $Button_1 = GUICtrlCreateLabel("Install", 30, 170, 100,25, BitOr($SS_NOTIFY, $SS_CENTER, $SS_CENTERIMAGE))
   $Button_2 = GUICtrlCreateLabel("Cancel", 410, 170, 100,25,BitOr($SS_NOTIFY, $SS_CENTER, $SS_CENTERIMAGE))
   GUICtrlSetFont($lab,15,Default,Default,"Trebuchet MS") ;zmiana czcionki
   GUICtrlSetFont($lab2,13,Default,Default,"Trebuchet MS") ;zmiana czcionki
   ;GUICtrlSetFont($lab3,13,Default,Default,"Trebuchet MS") ;zmiana czcionki
   GUICtrlSetFont($lab4,13,900,4,"Trebuchet MS") ;zmiana czcionki
   GUICtrlSetFont($Button_1,12,Default,Default,"Trebuchet MS") ;zmiana czcionki
   GUICtrlSetFont($Button_2,12,Default,Default,"Trebuchet MS") ;zmiana czcionki
   StyleToggle(1)
   $checkbox =  GUICtrlCreateCheckbox("I have read and agree to", 30, 110,200,30, $SS_LEFT)
   $checkbox2 =  GUICtrlCreateCheckbox("Watch a one minute HOW TO video!", 30, 140,360,20, $SS_LEFT)
   GUICtrlSetState($checkbox2, BitOr($GUI_ENABLE, $GUI_CHECKED))
   GUICtrlSetFont($checkbox,12,Default,Default,"Trebuchet MS") ;zmiana czcionki
   GUICtrlSetFont($checkbox2,12,Default,Default,"Trebuchet MS") ;zmiana czcionki
   GUICtrlSetCursor($lab4,0)
   GUICtrlSetCursor($Button_1,0)
   GUICtrlSetCursor($Button_2,0)
   GUICtrlSetColor($checkbox,0x67A9D9)
   StyleToggle(0)
   GUICtrlSetColor($lab,0xffffff)
   GUICtrlSetBkColor($lab,0x67A9D9)
   GUICtrlSetColor($lab2,0x67A9D9)
   ;GUICtrlSetColor($lab3,0x67A9D9)
   GUICtrlSetColor($lab4,0x67A9D9)
   GUICtrlSetColor($Button_1,0xffffff)
   GUICtrlSetColor($Button_2,0xffffff)
   GUICtrlSetBkColor($Button_1,0x67A9D9)
   GUICtrlSetBkColor($Button_2,0x67A9D9)
   GUICtrlSetColor($checkbox,0x67A9D9)
   GUICtrlSetColor($checkbox2,0x67A9D9)
   GUICtrlSetOnEvent($Button_1, "OKButton")
   GUICtrlSetOnEvent($Button_2, "closePopup")
   GUICtrlSetOnEvent($lab4, "Terms")
   GUICtrlSetOnEvent($checkbox, "Terms2")
   GUISetOnEvent($GUI_EVENT_CLOSE, "closePopup")
   GUISetState(@SW_SHOW, $hui)


   While 1
	   Sleep(1)

	   if $status = True and GUICtrlRead($checkbox)=$GUI_CHECKED Then
		  $statusvideo = GUICtrlRead($checkbox2)
		  GUIDelete($hui)
		  return True
	   EndIf

   WEnd

   Exit

EndFunc

Func installprogress()
	$hui = GUICreate("GarageFarm.NET Installer", 430, 130)
	GUISetIcon(@TempDir & "\uploader.ico", 0)
	GUISetBkColor(0xffffff)
	$lab2=GUICtrlCreateLabel("Installation in progress...", 10, 30,420,70,$SS_LEFT)

	$progressbar1 = GUICtrlCreateProgress(10, 70, 410, 40,1,0)
	GUICtrlSetData($progressbar1, 50)
	GUICtrlSetFont($lab2,16,Default,Default,"Trebuchet MS") ;zmiana czcionki
	GUICtrlSetColor($lab2,0x67A9D9)
	GUISetOnEvent($GUI_EVENT_CLOSE, "closePopup")
	GUISetState(@SW_SHOW, $hui)
EndFunc

Func installdonecopernicus()
	$hui = GUICreate("GarageFarm.NET Installer", 300, 130)
	AutoItSetOption("GUIOnEventMode", 1)

	GUISetIcon(@TempDir & "\uploader.ico", 0)
	GUISetBkColor(0xffffff)

	$lab2=GUICtrlCreateLabel("Installation complete!", 10, 30,290,40,1)

	$BTN = GUICtrlCreateLabel("Got it, start renderbeamer", 50, 70, 200,25, BitOr($SS_NOTIFY, $SS_CENTER, $SS_CENTERIMAGE))

	GUICtrlSetFont($lab2,16,Default,Default,"Arial") ;zmiana czcionki
	GUICtrlSetColor($lab2,0x67A9D9)
	GUICtrlSetColor($BTN,0xffffff)
	GUICtrlSetBkColor($BTN,0x67A9D9)
	GUICtrlSetOnEvent($BTN, "openBeamer")
	GUISetOnEvent($GUI_EVENT_CLOSE, "closePopup")
	GUICtrlSetCursor($BTN,0)

	GUISetState(@SW_SHOW, $hui)

	While 1
	sleep(1)

	WEnd

EndFunc

Func StyleToggle($Off = 1)

        If Not StringInStr(@OSType, "WIN32_NT") Then Return 0

        If $Off Then

                $XS_n = DllCall("uxtheme.dll", "int", "GetThemeAppProperties")

                DllCall("uxtheme.dll", "none", "SetThemeAppProperties", "int", 0)

                Return 1

        ElseIf IsArray($XS_n) Then

                DllCall("uxtheme.dll", "none", "SetThemeAppProperties", "int", $XS_n[0])

                $XS_n = ""

                Return 1

        EndIf

        Return 0

EndFunc

Func installRenderbeamer()
   GUICtrlSetData($progressbar1, 40)
   FileDelete(@UserProfileDir&"\renderbeamer\cc")
   DirRemove(@UserProfileDir&"\renderbeamer\jre8",1)
   DirRemove(@UserProfileDir&"\renderbeamer\dist",1)

   DirCreate(@UserProfileDir&"\renderbeamer")
   FileInstall("uploader.jar", @UserProfileDir&"\renderbeamer\uploader.jar",1)
   FileInstall("updater.jar", @UserProfileDir&"\renderbeamer\updater.jar",1)
   ;$filecopernicus = FileOpen(@UserProfileDir&"\renderbeamer\cc", 2)
   ;FileClose($filecopernicus)

   FileInstall("uploader.ico", @UserProfileDir&"\renderbeamer\uploader.ico",1)
   FileInstall("Uninstall renderbeamer.exe", @UserProfileDir&"\renderbeamer\Uninstall renderbeamer.exe",1)
   DirCreate(@UserProfileDir&"\renderbeamer\jre8")
   GUICtrlSetData($progressbar1, 60)
   DirCopy ( ".\jre8\", @UserProfileDir&"\renderbeamer\jre8", $FC_OVERWRITE )
   GUICtrlSetData($progressbar1, 90)

EndFunc

Func createShortcut()
   FileCreateShortcut(@UserProfileDir&"\renderbeamer\jre8\bin\renderbeamer.exe", "C:\Users\Public\Desktop\renderbeamer.lnk","",'-jar "'&@UserProfileDir&'\renderbeamer\uploader.jar"',"",@UserProfileDir&"\renderbeamer\uploader.ico")
   DirCreate(@UserProfileDir&"\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\renderbeamer")
   FileCreateShortcut(@UserProfileDir&"\renderbeamer\jre8\bin\renderbeamer.exe", @UserProfileDir&"\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\renderbeamer\renderbeamer.lnk","",'-jar "'&@UserProfileDir&'\renderbeamer\uploader.jar"',"",@UserProfileDir&"\renderbeamer\uploader.ico")
   FileCreateShortcut(@UserProfileDir&"\renderbeamer\Uninstall renderbeamer.exe", @UserProfileDir&"\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\renderbeamer\Uninstall renderbeamer.lnk")
EndFunc

Func savePluginVersion($app)
   DirCreate(@UserProfileDir&"\renderbeamer\plugins")
   $file = FileOpen(@UserProfileDir&"\renderbeamer\plugins\"&$app, 2)
   FileWrite($file, $VERSION & @CRLF) ; The CRLF at the end is a line break
   FileClose($file)
EndFunc

Func addFirevall()
   FileInstall("firewall_add.exe", @TempDir &"\firewall_add.exe",1)
   ShellExecute(@TempDir &"\firewall_add.exe", @UserProfileDir&"\renderbeamer\jre8\bin\renderbeamer.exe")
EndFunc

Func registerApp()
   RegWrite("HKEY_CLASSES_ROOT\gfrb","" , "REG_SZ", "URL:GarageFarm.NET RenderBeamer Protocol")
   RegWrite("HKEY_CLASSES_ROOT\gfrb", "URL Protocol", "REG_SZ", '')
   RegWrite("HKEY_CLASSES_ROOT\gfrb\shell\open\command", "", "REG_SZ", '"'&@UserProfileDir&'\renderbeamer\jre8\bin\renderbeamer.exe" -jar "'&@UserProfileDir&'\renderbeamer\uploader.jar"')
EndFunc


AutoItSetOption("GUIOnEventMode", 1)
If ProcessExists("renderbeamer.exe") or ProcessExists("garagefarm.exe") Then ; Check if the Notepad process is running.
   $hui = GUICreate("", 0, 0)
   GUISetIcon(@TempDir & "\uploader.ico", 0)
   GUISetState()
   MsgBox(0, "GarageFarm.NET Installer", "Application is open. Please run the installer again once the application is closed.",0,$hui)
   Exit
EndIf




#CS $versionCinema = StringMid($FileList[$FileList[0]],12,2)
### if $versionCinema <15 Then
###    $hui = GUICreate("", 0, 0)
###    GUISetIcon(@TempDir & "\uploader.ico", 0)
###    GUISetState()
###    MsgBox(0, "GarageFarm.NET Installer", "Your Cinema here is not supported. Please update at least to R15.",0,$hui)
###    Exit
### EndIf
 #CE

TraySetState(2)
Global $XS_n
Global $checkbox
Global $progressbar1 = GUICtrlCreateProgress(10, 100, 410, 40,1,0)
Global $hui = GUICreate("GarageFarm.NET Installer", 430, 200)
installstart()
installprogress()
GUICtrlSetData($progressbar1, 10)

installRenderbeamer()

;After Effects
For $i = 1 To 100
   $sSubKey = RegEnumKey("HKLM64\SOFTWARE\Adobe\After Effects", $i)
   If @error <> 0 Then ExitLoop
   $var = ""
   $var = RegRead("HKLM64\SOFTWARE\Adobe\After Effects\"&$sSubKey, "InstallPath")
   if $var <> "" Then
	  $path = $var&"Scripts\renderbeamer.jsxbin"
	  FileInstall("renderbeamer.jsxbin", $path,1)
	  $path = $var&"Scripts\renderbeamer Cost Calculator.jsxbin"
	  FileInstall("renderbeamer Cost Calculator.jsxbin", $path,1)
   EndIf



Next

DirCreate(@UserProfileDir&"\renderbeamer\AfterEffects")
FileInstall ( ".\dist\renderbeamer AfterEffects.exe", @UserProfileDir&"\renderbeamer\AfterEffects\renderbeamer AfterEffects.exe", 1 )





GUICtrlSetData($progressbar1, 95)
savePluginVersion("AfterEffects")
createShortcut()
addFirevall()
registerApp()

GUICtrlSetData($progressbar1, 100)
GUIDelete ($hui)
installdone();end
