#include "AeFileReferenceInterfaceHelper.h"

std::vector<AeFileNode*> &FileReferenceInterfaceHelper::GetUniqueFilesContainer()
{
	static std::vector<AeFileNode*> uniqueFilesContainer;
	return uniqueFilesContainer;
}
unsigned long long &FileReferenceInterfaceHelper::GetUniqueFilesTotalSize()
{
	static unsigned long long uniqueFilesTotalSize;
	return uniqueFilesTotalSize;
}

unsigned long FileReferenceInterfaceHelper::PushUniqueFilePath(AeFileNode *node)
{
	unsigned long i = 0;
	
	while (i < GetUniqueFilesContainer().size()) 
	{
		if(node->operator==(*GetUniqueFilesContainer()[i]))
		{
			delete node;
			node = nullptr;
			return i;
		}
		++i;
	} 
	GetUniqueFilesTotalSize() += node->GetNodeFilesSize();
	GetUniqueFilesContainer().push_back(node);
	return i;
}

void FileReferenceInterfaceHelper::ListUniqueFiles()
{
	unsigned long i = 0;
	for (auto *pt : GetUniqueFilesContainer())
	{
		std::cout << i++ << ": ";
		pt->ListNodeMainInfo();
	}
}
AeFileNode *FileReferenceInterfaceHelper::GetUniqueFileNode(unsigned long nodeNr)
{
	if (nodeNr < GetUniqueFilesContainer().size())
		return GetUniqueFilesContainer()[nodeNr];
	return nullptr;
}
long FileReferenceInterfaceHelper::GetLongFilesUID(std::string uid)
{
	if (uid.size() != 8)
		return 0;
	std::string tmp_uid;
	for (auto it = uid.rbegin(); it != uid.rend(); ++it)
	{
		tmp_uid.push_back(*(it + 1));
		tmp_uid.push_back(*(it++));
	}
	return strtol(tmp_uid.c_str(), nullptr, 16);
}
