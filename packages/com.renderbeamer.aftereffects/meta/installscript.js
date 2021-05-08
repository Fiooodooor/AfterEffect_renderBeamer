/**************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
**************************************************************************/

var targetDirectoryPage = null;

function Component() 
{
	component.addStopProcessForUpdateRequest("AfterFX.exe");	
    installer.gainAdminRights();
    component.loaded.connect(this, this.installerLoaded);
	
}


Component.prototype.createOperationsForArchive = function(archive)
{
	
	component.addOperation("Extract", archive, QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects");
	component.addOperation("Copy", QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects/data/AfterEffects", QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+"/renderbeamer/plugins/AfterEffects");

	if (systemInfo.kernelType === "winnt") 
	{		
		lastPath = ""
		fromPath = QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects/data/GF_AEGP_Renderbeamer.aex"
		fromPath2 = QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects/data/C4dRelinkerLibrary.dll"
		
		var keyName = "CommonPluginInstallPath";
		var typeDat = "REG_SZ";
		
		for (i = 14; i < 19; i++) 
		{
			var regPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe\\After Effects\\" + i.toString() + ".0";
			
			var registryQueryResult = installer.execute("reg", new Array("QUERY", regPath, "/v", keyName, "/reg:64"))[0];
			if(registryQueryResult != null)
			{
				afterPath = registryQueryResult.replace(regPath, "").replace(keyName, "").replace(typeDat, "").trim();
				if (installer.fileExists(afterPath) && afterPath != lastPath  )
				{
					lastPath = afterPath;
					component.addOperation("Copy", fromPath, afterPath+"//GF_AEGP_Renderbeamer.aex");
					component.addOperation("Copy", fromPath2, afterPath+"//C4dRelinkerLibrary.dll");
				}
			}
				
		}
		
		UIpath = QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects/data/renderBeamerUI";
		UIpathDir = QDesktopServices.storageLocation(QDesktopServices.HomeLocation) +"/AppData/Roaming/Adobe/CEP/extensions/renderBeamerUI";
		UIpathDir_old = QDesktopServices.storageLocation(QDesktopServices.HomeLocation) +"/AppData/Roaming/Adobe/CEP/extensions/com.acc.renderBeamerUI";
		if (installer.fileExists(UIpathDir_old))
		{
			var re_old = new RegExp("/", 'g');
			path3_old = UIpathDir_old.replace(re_old, '\\');
			component.addElevatedOperation("Execute", ["cmd", "/C", "del", "/Q", "/S", "/F", path3_old, "UNDOEXECUTE", "cmd", "/C"]);
		}
		if (installer.fileExists(UIpathDir))
		{
			var re = new RegExp("/", 'g');
			path3 = UIpathDir.replace(re, '\\');
			component.addElevatedOperation("Execute", ["cmd", "/C", "del", "/Q", "/S", "/F", path3, "UNDOEXECUTE", "cmd", "/C"]);
		}
		
		//if (!installer.fileExists(UIpathDir))
		//{
			//component.addOperation("Mkdir", UIpathDir);
		//}
		
		//component.addOperation("CopyDirectory", UIpath, UIpathDir);
	
	}
	else if (systemInfo.kernelType === "darwin") 
	{
		component.addOperation("CopyDirectory", [QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects/data/renderBeamer.plugin", "/Library/Application Support/Adobe/Common/Plug-ins/7.0/MediaCore/", "forceOverwrite"]);
		if (installer.fileExists("/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI"))
		{
			component.addElevatedOperation("Execute", ["rm", "-R", "/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI/"])
		}
		if (installer.fileExists("/Library/Application Support/Adobe/CEP/extensions/com.acc.renderBeamerUI/"))
		{
			component.addElevatedOperation("Execute", ["rm", "-R", "/Library/Application Support/Adobe/CEP/extensions/com.acc.renderBeamerUI/"])
		}
		//if (!installer.fileExists("/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI/"))
		//{
		//	component.addOperation("Mkdir", "/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI/");
		//}		
		//component.addOperation("CopyDirectory", [QDesktopServices.storageLocation(QDesktopServices.HomeLocation)+ "/renderbeamer/uninstall/AfterEffects/data/renderBeamerUI", "/Library/Application Support/Adobe/CEP/extensions/", "forceOverwrite"]);
	}
		

}

Component.prototype.installerLoaded = function()
{
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);

    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");

    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);

    gui.pageById(QInstaller.ComponentSelection).entered.connect(this, this.componentSelectionPageEntered);
}

Component.prototype.targetChooserClicked = function()
{
    var dir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);
    targetDirectoryPage.targetDirectory.setText(dir);
}

Component.prototype.targetDirectoryChanged = function()
{
    var dir = targetDirectoryPage.targetDirectory.text;
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Existing installation detected and will be overwritten.</p>");
    }
    else if (installer.fileExists(dir)) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Installing in existing directory. It will be wiped on uninstallation.</p>");
    }
    else {
        targetDirectoryPage.warning.setText("");
    }
    installer.setValue("TargetDir", dir);
}

Component.prototype.componentSelectionPageEntered = function()
{
    var dir = installer.value("TargetDir");
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        installer.execute(dir + "/maintenancetool.exe", "--script=" + dir + "/scripts/auto_uninstall.qs");
    }
}

