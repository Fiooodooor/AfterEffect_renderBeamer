function Component() { }
Component.prototype.createOperationsForArchive = function(archive)
{
    component.addOperation("Extract", archive, "@TargetDir@");
    installer.setInstallerBaseBinary("@TargetDir@/maintenancetool.exe"); 
}