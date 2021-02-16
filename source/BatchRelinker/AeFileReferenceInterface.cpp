#include "AeFileReferenceInterface.h"

//------------------------------------------------------------------------------------------------
//
//			SingleFileReference
//		
//------------------------------------------------------------------------------------------------

void SingleFileReference::AddFile(const fs::path &fileName)
{
	SetMainFilesPath(fs::path(fileName).remove_filename());
	auto *node = new AeFileNode(false, filesUID, GetMainFilePath());
	node->PushSourceFilename(new AeFileNode::FilenameCouple(false, 0, GetFilesReferencePointer(), fileName.filename().string()));
	nodeId = FileReferenceInterfaceHelper::PushUniqueFilePath(node);
}

bool SingleFileReference::RelinkFiles()
{
	auto *node = FileReferenceInterfaceHelper::GetUniqueFileNode(nodeId);
	if (node != nullptr)
	{
		GetFilesReferencePointer()->SetAttribute("fullpath", node->GetFileFullRelinkPath().lexically_normal().string().c_str());
		GetFilesReferencePointer()->SetAttribute("platform", PATHPLATFORM);
		GetFilesReferencePointer()->SetAttribute("server_name", "GarageFarm");
		GetFilesReferencePointer()->SetAttribute("ascendcount_base", "1");
		GetFilesReferencePointer()->SetAttribute("ascendcount_target", "2");
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------
//
//			SequenceListFileReference
//		
//------------------------------------------------------------------------------------------------

void SequenceListFileReference::AddFile(tinyxml2::XMLElement *fileReference)
{
	if (fileReference)
	{
		auto *node = new AeFileNode(true, filesUID, GetMainFilePath());
		auto filesToLookFor = std::strtol(fileReference->Attribute("bdata"), nullptr, 16);
		if (filesToLookFor > 59999 || filesToLookFor < 0)
			filesToLookFor = 59999;
			
		node->SetMaxFilesCount(filesToLookFor);

		fileReference = fileReference->NextSiblingElement();
		while (fileReference)
		{			
			if (std::string(fileReference->Value()).compare("string") == 0)			
				node->PushSourceFilename(new AeFileNode::FilenameCouple(false, 0, fileReference, fileReference->GetText()));			
			fileReference = fileReference->NextSiblingElement();
		}
		nodeId = FileReferenceInterfaceHelper::PushUniqueFilePath(node);
	}
}

bool SequenceListFileReference::RelinkFiles()
{
	auto *node = FileReferenceInterfaceHelper::GetUniqueFileNode(nodeId);
	if (node != nullptr)
	{
		GetFilesReferencePointer()->SetAttribute("fullpath", node->GetFileRelinkPath().lexically_normal().string().c_str());
		GetFilesReferencePointer()->SetAttribute("platform", PATHPLATFORM);
		GetFilesReferencePointer()->SetAttribute("server_name", "GarageFarm");
		GetFilesReferencePointer()->SetAttribute("ascendcount_base", "1");
		GetFilesReferencePointer()->SetAttribute("ascendcount_target", "2");

		for (unsigned long long i = 0; i < node->GetFilenamesNumber(); ++i)
		{
			const auto pt = node->GetFilenameCouple(i);
			if(pt != nullptr && pt->xmlFileReference != nullptr)
				pt->xmlFileReference->FirstChild()->ToText()->SetValue(pt->relinkedFileName.c_str());
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------
//
//			SequenceMaskFileReference
//		
//------------------------------------------------------------------------------------------------

void SequenceMaskFileReference::AddFilesByMask(tinyxml2::XMLElement *base, tinyxml2::XMLElement *extension)
{
	filesMaskBase = base;
	filesMaskExtension = extension;

	const std::string  uStringMaskBase = base->GetText();	
	const std::string  uStringMaskExtension = extension->GetText();
	FS_ERROR_CODE(fileRefError)

	auto *node = new AeFileNode(true, filesUID, GetMainFilePath(), uStringMaskBase);
	
	for (auto p : fs::directory_iterator(GetMainFilePath()))
	{
		const fs::file_status entryStatus(p.status(fileRefError));
		if (fileRefError.value() == 0 && fs::exists(entryStatus))
		{
			fs::path fileEntryPath(p.path());
			if (fs::is_symlink(entryStatus)) {
				fileEntryPath = fs::read_symlink(p, fileRefError);
				if (fileRefError.value() != 0) {
					fileEntryPath = p.path();
					fileRefError.clear();
				}
			}

			if (fileEntryPath.has_extension() && fileEntryPath.extension().compare(uStringMaskExtension) == 0)
			{
				if (fileEntryPath.has_filename() && fileEntryPath.filename().string().size() > uStringMaskBase.size())
				{
					if (uStringMaskBase.compare(0, std::string::npos, fileEntryPath.filename().string(), 0, uStringMaskBase.size()) == 0)
					{
						auto file_size = fs::file_size(fileEntryPath, fileRefError);
						if (fileRefError.value() != 0) {
							file_size = 0;
							fileRefError.clear();
						}
						node->PushSourceFilename(new AeFileNode::FilenameCouple(false, file_size, nullptr, fileEntryPath.filename().string()));
					}
				}
			}
		}
	}
	nodeId = FileReferenceInterfaceHelper::PushUniqueFilePath(node);
}

bool SequenceMaskFileReference::RelinkFiles()
{
	auto *node = FileReferenceInterfaceHelper::GetUniqueFileNode(nodeId);
	if(node != nullptr)
	{		
		GetFilesReferencePointer()->SetAttribute("fullpath", node->GetFileRelinkPath().lexically_normal().string().c_str());
		GetFilesReferencePointer()->SetAttribute("platform", PATHPLATFORM);
		GetFilesReferencePointer()->SetAttribute("server_name", "GarageFarm");
		GetFilesReferencePointer()->SetAttribute("ascendcount_base", "1");
		GetFilesReferencePointer()->SetAttribute("ascendcount_target", "2");

		filesMaskBase->FirstChild()->ToText()->SetValue(node->GetSequenceMaskRelinkedBase().c_str());
		return true;
	}
	return false;
}