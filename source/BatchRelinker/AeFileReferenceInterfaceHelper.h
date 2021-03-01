#pragma once

#include "AeFileNode.h"

class FileReferenceInterfaceHelper
{
public:
	static long GetLongFilesUID(std::string uid);
	static void ListUniqueFiles();
	static unsigned long PushUniqueFilePath(AeFileNode *fileNode);
	
	static std::vector<AeFileNode*> &GetUniqueFilesContainer();
	static unsigned long long &GetUniqueFilesTotalSize();
	static AeFileNode *GetUniqueFileNode(unsigned long nodeNr);
};

