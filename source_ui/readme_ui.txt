
WINDOWS:
* Copy the extracted UI folder to (create if one does not exists):
	c:\Users\[user name]\AppData\Roaming\Adobe\CEP\extensions\

* Regedit the registry key listed bellow and add a key named PlayerDebugMode, of type String, and value 1: 
	HKEY_CURRENT_USER/Software/Adobe/CSXS.6 

MAC:
* Copy the extracted UI folder to (create if one does not exists):
	/Library/Application\ Support/Adobe/CEP/extensions/

* Open the file listed bellow and add a row with key PlayerDebugMode, of type String, and value 1:
	~/Library/Preferences/com.adobe.CSXS.6.plist