#RequireAdmin

#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.8.1
 Author:         myName

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------

; Script Start - Add your code below here
#include <File.au3>
#include <Array.au3>


;After Effects
For $i = 1 To 100
   $sSubKey = RegEnumKey("HKLM64\SOFTWARE\Adobe\After Effects", $i)
   If @error <> 0 Then ExitLoop
   $var = ""
   $var = RegRead("HKLM64\SOFTWARE\Adobe\After Effects\"&$sSubKey, "InstallPath")
   if $var <> "" Then
	  $path = $var&"Scripts\renderbeamer.jsxbin"
	  FileInstall("renderbeamer.jsxbin", $path,1)
   EndIf



Next

DirCreate(@UserProfileDir&"\renderbeamer\AfterEffects")
FileInstall ( ".\dist\renderbeamer AfterEffects.exe", @UserProfileDir&"\renderbeamer\AfterEffects\renderbeamer AfterEffects.exe", 1 )

DirCreate(@UserProfileDir&"\renderbeamer\plugins")
$file = FileOpen(@UserProfileDir&"\renderbeamer\plugins\AfterEffects", 2)
FileWrite($file, "2" & @CRLF) ; The CRLF at the end is a line break
FileClose($file)


