#include "AeFooNode.h"
//AEGP_FootageSignature_MISSING
AeFootageNode::AeFootageNode(AeObjectNode* objNode) : AeObjectNode(objNode)
{
	AEGP_SuiteHandler suites(sp);
	suites.FootageSuite5()->AEGP_GetMainFootageFromItem(itemH, &fooH);
	suites.FootageSuite5()->AEGP_GetFootageSignature(fooH, &signature);
	suites.FootageSuite5()->AEGP_GetFootageNumFiles(fooH, &nrFiles, &nrFpFrame);
	copyOf = -1;
}
AEGP_FootageH AeFootageNode::getFooH() const
{ 
	return fooH;
}
A_long AeFootageNode::getNrFiles() const
{
	return nrFiles;
}
bool AeFootageNode::isFooFile() const
{
	if (signature == AEGP_FootageSignature_NONE || signature == AEGP_FootageSignature_SOLID)
		return false;
	if (nrFiles > 0)
		return true;
	else
		return false;
}
bool AeFootageNode::isFooMissing() const
{
	return signature == AEGP_FootageSignature_MISSING;
}
bool AeFootageNode::isFooSequence() const
{ 
	return (seqImpOptions.all_in_folderB == TRUE);
}
long AeFootageNode::generateFootData()
{
	AEGP_SuiteHandler suites(sp);
	AEGP_MemHandle memoryH = nullptr;

	suites.FootageSuite5()->AEGP_GetFootagePath(getFooH(), 0, AEGP_FOOTAGE_MAIN_FILE_INDEX, &memoryH);

	rbUtilities::copyMemhUTF16ToPath(getSp(), memoryH, path);
	oryginalPath = path.wstring();
//	rbUtilities::copyMemhUTF16ToString(getSp(), memoryH, oryginalPath);
//	path = oryginalPath;

	if (isFooFile() && !isFooMissing())
	{
		suites.FootageSuite5()->AEGP_GetFootageSequenceImportOptions(getFooH(), &seqImpOptions);
		suites.FootageSuite5()->AEGP_GetFootageInterpretation(getItemH(), FALSE, &interpret);
		suites.FootageSuite5()->AEGP_GetFootageLayerKey(getFooH(), &layerKey);
	}
	return 0;
}
