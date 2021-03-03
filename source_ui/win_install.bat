@echo off
if not exist %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\com.acc.renderBeamerUI mkdir %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\com.acc.renderBeamerUI
xcopy /s %~dp0\com.acc.renderBeamerUI %UserProfile%\AppData\Roaming\Adobe\CEP\extensions\com.acc.renderBeamerUI
REG ADD "HKEY_CURRENT_USER\Software\Adobe\CSXS.7" /v "PlayerDebugMode" /t REG_SZ /d 1 /f