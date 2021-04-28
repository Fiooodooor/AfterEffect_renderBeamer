#include "AeBatchRelinker.h"
#include <clocale>
#include <algorithm>

namespace RenderBeamer {
AeBatchRelinker::AeBatchRelinker(SPBasicSuite* pb, PlatformLibLoader* c4dLoader, rbProjectClass* rbLogger, PF_AppProgressDialogP* progressD, const fs::path& aepxPath)
		: picaBasic(pb)
        , libC4dPointer(c4dLoader)
		, rbProjLogger(rbLogger)
        , progressDialog(progressD)
		, aepxXmlDocumentPath(aepxPath.lexically_normal())
		, unique_files_total_size(0)
{
    
}
AeBatchRelinker::~AeBatchRelinker()
{
	while(!fileItemNodes.empty())
	{
		delete fileItemNodes.back();
		fileItemNodes.pop_back();
	}
	while (!unique_file_nodes.empty())
	{
		delete unique_file_nodes.back();
		unique_file_nodes.pop_back();
	}
}
ErrorCodesAE AeBatchRelinker::ParseAepxXmlDocument()
{
	ERROR_CATCH_START
	AEGP_SuiteHandler suites(picaBasic);
	rbProjLogger->logg("AeBatchRelinker", "ParseAepxXmlDocument", "Starting scene batch parsing function.");
	
	unique_files_total_size = 0;
	if (aepxXmlDocument.LoadFile(aepxXmlDocumentPath.string().c_str()) != tinyxml2::XML_NO_ERROR)
		return ErrorResult;
    
	tinyxml2::XMLElement *AepxXmlElement = aepxXmlDocument.FirstChildElement();
	FileReferenceInterface *fileReference;
	while (AepxXmlElement)
	{
        MAIN_PROGRESS_THROW(*progressDialog, 0, 10)
		if (AepxXmlElement->Value() && std::string(AepxXmlElement->Value()) == std::string("fileReference"))
		{
			if (AepxXmlElement->Parent() && AepxXmlElement->Parent()->Parent() && std::string(AepxXmlElement->Parent()->Parent()->Value()) == std::string("Pin"))
			{
				MAIN_PROGRESS_THROW(*progressDialog, 0, 10)
	
				rbProjLogger->logg("BatchAepxParser", "Path", AepxXmlElement->Attribute("fullpath"));
				fileReference = CreateFileReference(AepxXmlElement);
    
				if (fileReference != nullptr)
					fileItemNodes.push_back(fileReference);
				else
					rbProjLogger->loggErr("BatchAepxParser", "FileReferenceNullptr", "File parsing error! Returned nullptr while creating reference.");
				MAIN_PROGRESS_THROW(*progressDialog, 0, 10)
			}
		}
		if (AepxXmlElement->FirstChildElement())
			AepxXmlElement = AepxXmlElement->FirstChildElement();
		else if (AepxXmlElement->NextSiblingElement())
			AepxXmlElement = AepxXmlElement->NextSiblingElement();
		else
		{
            while (AepxXmlElement->Parent()->ToElement() && !AepxXmlElement->Parent()->NextSiblingElement())
				AepxXmlElement = AepxXmlElement->Parent()->ToElement();
    
			if (AepxXmlElement->Parent()->ToElement() && AepxXmlElement->Parent()->NextSiblingElement())
				AepxXmlElement = AepxXmlElement->Parent()->NextSiblingElement();
			else
				break;
		}
    }
    rbProjLogger->logg("BatchAepxParser", "End", "End of parsing function - no error return.");
    
	ERROR_CATCH_END_LOGGER2("AeBatchRelinker", "ParseAepxXmlDocument", rbProjLogger)
    return _ErrorCode;
}

FileReferenceInterface *AeBatchRelinker::CreateFileReference(tinyxml2::XMLElement *fileReferencePt)
{
    const std::lock_guard<std::mutex> lock(m);
	FileReferenceInterface *fileRef = nullptr;
	AeFileNodeID node_id = 0;
	std::string fileUid = "00000000";
	fs::path FileBasePath;
	FS_ERROR_CODE(fileRefError)
    FS_ERROR_ASSIGN(fileRefError,100)

	ERROR_CATCH_START
	FileBasePath = fs::absolute(fs::path(fileReferencePt->Attribute("fullpath"))).lexically_normal();
	int ascendcount_base = fileReferencePt->IntAttribute("ascendcount_base");
	int ascendcount_target = fileReferencePt->IntAttribute("ascendcount_target");
	
	try {			
		const fs::file_status FileBasePathStatus = status(FileBasePath);
		if (fs::status_known(FileBasePathStatus) ? !fs::exists(FileBasePathStatus) : !fs::exists(FileBasePath))
			throw fs::filesystem_error("File does not exist! Trying to AE style path resolve ", fileRefError);
		if (FileBasePathStatus.type() == FS_TYPE_UNKNOWN) {
			rbProjLogger->loggErr("CreateFileReference", "FS_TYPE_UNKNOWN", "Path exists but is inaccessible. File will probably not success copy. Try running AE with admin rights.");
		}
		if (FileBasePathStatus.type() == FS_TYPE_NONE) {
			rbProjLogger->loggErr("CreateFileReference", "FS_TYPE_NONE", "There was an error while resolving file status. File will probably not success copy.");
		}
		if (fs::is_symlink(FileBasePath)) {
			rbProjLogger->logg("CreateFileReference", "FS_TYPE_SYMLINK", "Path exists and is a symlink. Resolving.");
			FileBasePath = fs::read_symlink(FileBasePath);
		}
	}
	catch(fs::filesystem_error &e) {
		rbProjLogger->loggErr("BatchAepxParser", "FullPath", e.what());
		
		if (ascendcount_base > 0 && ascendcount_target > 0)
		{
			FileBasePath = aepxXmlDocumentPath;
			std::string stringBaseRelative = fileReferencePt->Attribute("fullpath");
			#ifdef AE_OS_WIN
				std::replace(stringBaseRelative.begin(), stringBaseRelative.end(), '/', '\\');
			#else
				std::replace(stringBaseRelative.begin(), stringBaseRelative.end(), '\\', '/');
			#endif
		
			const fs::path FileBaseRelative = fs::absolute(fs::path(stringBaseRelative)).lexically_normal();

			auto it = FileBaseRelative.end();

			while (ascendcount_base-- && FileBasePath.has_parent_path())
				FileBasePath = FileBasePath.parent_path();
			while (it != FileBaseRelative.begin() && ascendcount_target--)
				--it;

			while (it != FileBaseRelative.end())
				FileBasePath /= (it++)->string();
			rbProjLogger->logg("BatchAepxParser", "ResolvedRelative", FileBasePath.string().c_str());
		}
		else
			return nullptr;
	}
	
	if (fileReferencePt->Parent()->Parent()->Parent() && fileReferencePt->Parent()->Parent()->Parent()->FirstChildElement()) {
		if (strcmp(fileReferencePt->Parent()->Parent()->Parent()->FirstChildElement()->Name(), "iide") == 0) {			
			fileUid = std::to_string(GetLongFilesUID(std::string(fileReferencePt->Parent()->Parent()->Parent()->FirstChildElement()->Attribute("bdata"))));	
		}	
	}

    if (fileReferencePt->Attribute("target_is_folder", "0")) {
        fileRef = new SingleFileReference(fileUid, fileReferencePt, FileBasePath);
    }
    else {
        tinyxml2::XMLElement *fileReferenceSequence = fileReferencePt->Parent()->NextSiblingElement();
        FileBasePath += fs::path::preferred_separator;

        if (fileReferenceSequence)
        {
            if (std::string(fileReferenceSequence->Value()) == "StVc")
            {
                fileReferenceSequence = fileReferenceSequence->FirstChildElement();
                if (fileReferenceSequence && std::string(fileReferenceSequence->Value()) == "StVS")
                {
                    fileRef = new SequenceListFileReference(fileUid, fileReferencePt, fileReferenceSequence);
                }
            }
            else if (std::string(fileReferenceSequence->Value()) == "string" && fileReferenceSequence->NextSiblingElement() && std::string(fileReferenceSequence->NextSiblingElement()->Value()) == "string")
            {
                fileRef = new SequenceMaskFileReference(fileUid, fileReferencePt, fileReferenceSequence, fileReferenceSequence->NextSiblingElement());
            }
        }
    }
	if (fileRef)
	{
		fileRef->SetMainFilesPath(FileBasePath);
		auto* node_pt = fileRef->AddFiles();
		if (node_pt != nullptr) {
			node_id = PushUniqueFilePath(node_pt);
			fileRef->SetNodeId(node_id);
		}
        else {
            throw PluginError(GF_PLUGIN_LANGUAGE, NullPointerResult);
        }
	}
	ERROR_CATCH_END_LOGGER2("BatchRelinker", "CreateFileReference", rbProjLogger)
	if (fileRef && _ErrorCode != NoError)
	{
		delete fileRef;
		fileRef = nullptr;
	}
	return fileRef;
}

ErrorCodesAE AeBatchRelinker::CopyAndRelinkFiles(const fs::path &localAssetsPath, const fs::path &remoteAssetsPath)
{
	ErrorCodesAE _error = CopyUniqueFiles(localAssetsPath.lexically_normal(), remoteAssetsPath.lexically_normal());
	if (_error == NoError)
	{
		if (!fileItemNodes.empty())
		{
			for (auto* pt : fileItemNodes) {
				AeFileNode* file_node = GetUniqueFileNode(pt->GetNodeId());
				pt->RelinkFiles(file_node);
			}
		}
		_error = aepxXmlDocument.SaveFile(aepxXmlDocumentPath.string().c_str()) == tinyxml2::XML_NO_ERROR ? NoError : ErrorResult;
	}
	return _error;
}

ErrorCodesAE AeBatchRelinker::CopyUniqueFiles(const fs::path &localCopyPath, const fs::path &remoteCopyPath)
{
	AEGP_SuiteHandler suites(picaBasic);
	FS_ERROR_CODE(copyError)
	ErrorCodesAE _ErrorCode = NoError;
	std::string relinkedFileName;
	unsigned long long i, relinkedFilesSize = 0, totalFilesSize = (GetUniqueFilesTotalSize() >> 10);

	if (unique_file_nodes.empty())
		return NoError;
	for (auto *node : unique_file_nodes)
	{
		node->SetFileCopyPath(localCopyPath);
		node->SetFileRelinkPath(remoteCopyPath);
		
		for(i=0; i < node->GetFilenamesNumber(); ++i)
		{
			auto pt = fs::path(node->GetFilenameCouple(i)->sourceFileName);
			MAIN_PROGRESS_RETURN(*progressDialog, static_cast<A_long>(relinkedFilesSize), static_cast<A_long>(totalFilesSize))
			
			if(FileExtensionCheck(pt, ".c4d"))
			{
				if (GF_AEGP_Relinker::GFCopy_C4D_File(libC4dPointer, node->GetFileFullSourcePath(i), node->GetFileFullCopyPath(i), node->GetFileFullRelinkPath(i), node->GetFileNodeUid()) == NoError)
					rbProjLogger->logg("BatchRelink::c4d", node->GetFileFullSourcePath(i).string().c_str(), node->GetFileFullCopyPath(i).string().c_str());
				else
					rbProjLogger->loggErr("BatchRelink::c4d", node->GetFileFullSourcePath(i).string().c_str(), node->GetFileFullCopyPath(i).string().c_str());
			}
			else
			{
                fs::copy_file(node->GetFileFullSourcePath(i), node->GetFileFullCopyPath(i), FS_COPY_OPTIONS, copyError);
				if(copyError.value() == 0)
				{
					rbProjLogger->logg("BatchRelink", "Copy", node->GetFileFullSourcePath(i).string().c_str());
				}
				else
				{
					// add files to list of errors
					// ...
					rbProjLogger->loggErr("BatchRelink::Copy", node->GetFileFullSourcePath(i).string().c_str(), node->GetFileFullCopyPath(i).string().c_str());
					rbProjLogger->loggErr("BatchRelink::Copy", std::to_string(copyError.value()).c_str(), copyError.message().c_str());
					copyError.clear();
				}
			}
			relinkedFilesSize += (node->GetFilenameCouple(i)->file_size >> 10);
		}
	}
	return NoError;
}
bool AeBatchRelinker::FileExtensionCheck(const fs::path& path_to_check, const fs::path& extension) const
{
	if (!path_to_check.has_extension())
		return false;
	if (path_to_check.extension().string().size() != extension.string().size())
		return false;
	for(unsigned long long i=0; i < extension.string().size(); ++i)
	{
		if (std::tolower(path_to_check.extension().string().c_str()[i]) != std::tolower(extension.string().c_str()[i]))
			return false;
	}
	return true;
}

AeFileNodeID AeBatchRelinker::PushUniqueFilePath(AeFileNode *node)
{
	unsigned long long i = 0;
	if (node == nullptr)
		return 0;
	
	while (i < unique_file_nodes.size())
	{
		if (node->operator==(*unique_file_nodes[i]))
		{
			delete node;
			node = nullptr;
			return i;
		}
		++i;
	}
	unique_files_total_size += node->GetNodeFilesSize();
	unique_file_nodes.push_back(node);
	return i;
}
AeFileNode *AeBatchRelinker::GetUniqueFileNode(AeFileNodeID nodeNr) const
{
	if (nodeNr < unique_file_nodes.size())
		return unique_file_nodes[nodeNr];
	return nullptr;
}

unsigned long long AeBatchRelinker::GetUniqueFilesTotalSize() const
{
	return unique_files_total_size;
}
A_long AeBatchRelinker::GetUniqueFilesTotalSizeA() const
{
	return static_cast<A_long>(GetUniqueFilesTotalSize());
}

void AeBatchRelinker::ListUniqueFiles() const
{
	unsigned long i = 0;
	if (unique_file_nodes.empty())
		return;
	for (auto *pt : unique_file_nodes)
	{
		std::cout << i++ << ": ";
		pt->ListNodeMainInfo();
	}
}
long AeBatchRelinker::GetLongFilesUID(std::string uid) const
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
} // namespace RenderBeamer
