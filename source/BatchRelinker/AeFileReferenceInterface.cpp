#include "AeFileReferenceInterface.h"

#include <utility>

#include "../GF_GlobalClasses.h"

namespace RenderBeamer {

//------------------------------------------------------------------------------------------------
//
//			FileReferenceInterface definition
//		
//------------------------------------------------------------------------------------------------

FileReferenceInterface::FileReferenceInterface(std::string theFilesUID, tinyxml2::XMLElement* theFileReferencePointer)
		: filesUID(std::move(theFilesUID))
		, fileReferencePointer(theFileReferencePointer)
		, nodeId(0)
{
}

void FileReferenceInterface::SetMainFilesPath(fs::path& mainPath)
{
	if (mainPath.has_filename())
		mainFilesPath = fs::path(mainPath).remove_filename();
	else
		mainFilesPath = mainPath;
}

fs::path FileReferenceInterface::GetMainFilePath() const
{
	return mainFilesPath;
}

//------------------------------------------------------------------------------------------------
//
//			SingleFileReference
//		
//------------------------------------------------------------------------------------------------

SingleFileReference::SingleFileReference(const std::string& theFilesUID, tinyxml2::XMLElement* theFileReferencePointer, fs::path fileName)
	: FileReferenceInterface(theFilesUID, theFileReferencePointer)
	, file_name(std::move(fileName))
{
}

AeFileNode* SingleFileReference::AddFiles()
{
	ERROR_CATCH_START
        auto *node = new AeFileNode(false, filesUID, GetMainFilePath());
        node->PushSourceFilename(new AeFileNode::FilenameCouple(false, 0, GetFilesReferencePointer(), file_name.filename().string()));
        return node;
	ERROR_CATCH_END_NO_INFO
	return nullptr;
}

bool SingleFileReference::RelinkFiles(AeFileNode* node)
{
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

SequenceListFileReference::SequenceListFileReference(const std::string& theFilesUID, tinyxml2::XMLElement* theFileReferencePointer, tinyxml2::XMLElement* fileReference)
	: FileReferenceInterface(theFilesUID, theFileReferencePointer)
	, file_reference(fileReference)
{
}

AeFileNode* SequenceListFileReference::AddFiles()
{
	ERROR_CATCH_START
	if (file_reference)
	{
		auto *node = new AeFileNode(true, filesUID, GetMainFilePath());
		auto filesToLookFor = std::strtol(file_reference->Attribute("bdata"), nullptr, 16);
		if (filesToLookFor <= 0)        // possible read or parsing error
			filesToLookFor = 16;
			
		node->SetMaxFilesCount(filesToLookFor);

		file_reference = file_reference->NextSiblingElement();
		while (file_reference)
		{			
			if (std::string(file_reference->Value()) == "string")
				node->PushSourceFilename(new AeFileNode::FilenameCouple(false, 0, file_reference, file_reference->GetText()));
			file_reference = file_reference->NextSiblingElement();
		}
		return node;
	}
	ERROR_CATCH_END_NO_INFO
	return nullptr;
}

bool SequenceListFileReference::RelinkFiles(AeFileNode* node)
{
	if (node != nullptr)
	{
		GetFilesReferencePointer()->SetAttribute("fullpath", node->GetFileRelinkPath().lexically_normal().string().c_str());
		GetFilesReferencePointer()->SetAttribute("platform", PATHPLATFORM);
		GetFilesReferencePointer()->SetAttribute("server_name", "GarageFarm");
		GetFilesReferencePointer()->SetAttribute("ascendcount_base", "1");
		GetFilesReferencePointer()->SetAttribute("ascendcount_target", "2");

		for (unsigned long long i = 0; i < node->GetFilenamesNumber(); ++i)
		{
			auto *const pt = node->GetFilenameCouple(i);
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

SequenceMaskFileReference::SequenceMaskFileReference(const std::string& theFilesUID, tinyxml2::XMLElement* theFileReferencePointer, tinyxml2::XMLElement *base, tinyxml2::XMLElement *extension)
		: FileReferenceInterface(theFilesUID, theFileReferencePointer)
		, files_mask_base(base)
		, files_mask_extension(extension)
{
}

AeFileNode* SequenceMaskFileReference::AddFiles()
{
	if (!files_mask_base || !files_mask_extension)
		return nullptr;
	
	const std::string  uStringMaskBase = files_mask_base->GetText();
	const std::string  uStringMaskExtension = files_mask_extension->GetText();
	FS_ERROR_CODE(fileRefError)

	auto *node = new AeFileNode(true, filesUID, GetMainFilePath(), uStringMaskBase);

	ERROR_CATCH_START
	for (auto const &p : fs::directory_iterator(GetMainFilePath()))
	{
		const fs::file_status entryStatus(p.status(fileRefError));
		if (fileRefError.value() == 0 && fs::exists(entryStatus))
		{
			auto const &fileEntryPath(p.path());

			// add extension check with letter size to_upper
			if(AeFileNode::FileExtensionCheck(fileEntryPath, uStringMaskExtension))
			{
				if (fileEntryPath.has_filename())
				{
					if(!uStringMaskBase.empty() && uStringMaskBase.compare(0, std::string::npos, fileEntryPath.filename().string(), 0, uStringMaskBase.size()) != 0)
						continue;
					
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
	return node;
	ERROR_CATCH_END_NO_INFO
	delete node;
	return nullptr;
}

bool SequenceMaskFileReference::RelinkFiles(AeFileNode* node)
{
	if(node != nullptr)
	{		
		GetFilesReferencePointer()->SetAttribute("fullpath", node->GetFileRelinkPath().lexically_normal().string().c_str());
		GetFilesReferencePointer()->SetAttribute("platform", PATHPLATFORM);
		GetFilesReferencePointer()->SetAttribute("server_name", "GarageFarm");
		GetFilesReferencePointer()->SetAttribute("ascendcount_base", "1");
		GetFilesReferencePointer()->SetAttribute("ascendcount_target", "2");

		A_char tmp_base[MAX_PATH] = { '\0' };
		size_t len = strlen(node->GetSequenceMaskRelinkedBase().c_str());
		memcpy(tmp_base, node->GetSequenceMaskRelinkedBase().c_str(), len + 1);
		rbUtilities::leaveAllowedOnly(tmp_base);
		
		files_mask_base->FirstChild()->ToText()->SetValue(tmp_base);
		return true;
	}
	return false;
}
} // namespace RenderBeamer
