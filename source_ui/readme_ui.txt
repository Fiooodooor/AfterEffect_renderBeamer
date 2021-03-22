
WINDOWS:
	just run win_install.bat  
	# directory com.acc.renderBeamerUI and c++ plugin files C4dRelinkerLibrary.dll, GF_AEGP_Renderbeamer.aex should be placed next to win_install.bat
	

MAC:	
	# Copy the extracted UI folder to (create if one does not exists):
		/Library/Application\ Support/Adobe/CEP/extensions/

	# (Option I) - using text editor:
	# Open the file listed bellow and add a row with key PlayerDebugMode, of type String, and value 1:
		~/Library/Preferences/com.adobe.CSXS.6.plist
	
	# (Option II) Open terminal (cmd), then:	
	# (1) List available CSXS engines:
		defaults domains | tr ',' '\n' | grep com.adobe.CSXS
		
	# (2) Now for each available engine type:
		defaults write com.adobe.CSXS.9 PlayerDebugMode -string 1
	