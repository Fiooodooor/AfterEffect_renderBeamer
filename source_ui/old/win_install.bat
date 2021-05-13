@echo off
if exist %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\com.acc.renderBeamerUI rm -r %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\com.acc.renderBeamerUI 
if not exist %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\renderBeamerUI mkdir %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\renderBeamerUI
xcopy /s %~dp0\renderBeamerUI %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\renderBeamerUI
xcopy /s %~dp0\C4dRelinkerLibrary.dll c:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\
xcopy /s %~dp0\GF_AEGP_Renderbeamer.aex c:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\
REG ADD "HKEY_CURRENT_USER\Software\Adobe\CSXS.6" /v "PlayerDebugMode" /t REG_SZ /d 1 /f
REG ADD "HKEY_CURRENT_USER\Software\Adobe\CSXS.7" /v "PlayerDebugMode" /t REG_SZ /d 1 /f
REG ADD "HKEY_CURRENT_USER\Software\Adobe\CSXS.8" /v "PlayerDebugMode" /t REG_SZ /d 1 /f
REG ADD "HKEY_CURRENT_USER\Software\Adobe\CSXS.9" /v "PlayerDebugMode" /t REG_SZ /d 1 /f
REG ADD "HKEY_CURRENT_USER\Software\Adobe\CSXS.10" /v "PlayerDebugMode" /t REG_SZ /d 1 /f