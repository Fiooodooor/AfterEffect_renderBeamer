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
	
				rbProjLogger->loggA(6, "AssetPath::SN::", AepxXmlElement->Attribute("server_name"), "::VN::", AepxXmlElement->Attribute("server_volume_name"), "::PTH::", AepxXmlElement->Attribute("fullpath"));
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
	FileReferenceInterface *fileRef = nullptr;
	AeFileNodeID node_id = 0;
	std::string fileUid = "0";
	
	FS_ERROR_CODE(fileRefError)

	ERROR_CATCH_START
	
	auto FileBasePath = CreateFilePath(fileReferencePt);
	if (FileBasePath.empty()) {
		rbProjLogger->loggErr("CreateFileReference", "ResolvedPath", "Resolved to empty path error!");
		return nullptr;
	}
	rbProjLogger->logg("CreateFileReference", "ResolvedPath", FileBasePath.string().c_str());
	
	const tinyxml2::XMLNode *item_reference = fileReferencePt->Parent()->Parent()->Parent();
	if (item_reference && item_reference->FirstChildElement("idta")) {
		fileUid = std::to_string(HexTo32Int(item_reference->FirstChildElement("idta")->Attribute("bdata"), 32));
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
            else if (std::string(fileReferenceSequence->Value()) == "string")
            {
				if (fileReferenceSequence->NextSiblingElement() && std::string(fileReferenceSequence->NextSiblingElement()->Value()) == "string") {
					fileRef = new SequenceMaskFileReference(fileUid, fileReferencePt, fileReferenceSequence, fileReferenceSequence->NextSiblingElement());
				}
				else {
					tinyxml2::XMLElement *new_base = aepxXmlDocument.NewElement("string");
					tinyxml2::XMLText *new_base_text = aepxXmlDocument.NewText("");
					new_base->InsertFirstChild(new_base_text);
					fileReferencePt->Parent()->Parent()->InsertAfterChild(fileReferencePt->Parent(), new_base);
					fileRef = new SequenceMaskFileReference(fileUid, fileReferencePt, new_base, fileReferenceSequence);
				}
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
		} else {
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
bool AeBatchRelinker::DoesFileExist(const fs::path &file_path)
{
	FS_ERROR_CODE(fs_error_code_)
	try {
		auto file_status = fs::status(file_path, fs_error_code_);
		if (fs_error_code_.value() != 0 || !fs::status_known(file_status))
		{
			if (fs::exists(file_path, fs_error_code_) && fs_error_code_.value() == 0)
				return true;
		}
		else if (fs::status_known(file_status))
		{
			if (fs::exists(file_status))
				return true;
		}
	}
	catch (fs::filesystem_error &e) {
		fs_error_code_ = e.code();
	}
	return false;
}
fs::path AeBatchRelinker::CreateFilePath(const tinyxml2::XMLElement *fileReferencePt) const
{
	fs::path base_path;
	ERROR_CATCH_START2
	if (!fileReferencePt)
		return "";
	
	rbProjLogger->loggA(8, "PathString", fileReferencePt->Attribute("platform"), fileReferencePt->Attribute("target_is_folder"), fileReferencePt->Attribute("server_name"), "VN", fileReferencePt->Attribute("server_volume_name"), "PTH", fileReferencePt->Attribute("fullpath"));

	FS_ERROR_CODE(fs_error_code_)
	int base = fileReferencePt->IntAttribute("ascendcount_base");
	int target = fileReferencePt->IntAttribute("ascendcount_target");
	
	bool is_windows_platform_ = (fileReferencePt->Attribute("platform", "Win") ? true : false);
	bool is_target_folder_ = (fileReferencePt->Attribute("target_is_folder", "0") ? true : false);
	
	std::string server_name(fileReferencePt->Attribute("server_name"));
	std::string server_volume_name(fileReferencePt->Attribute("server_volume_name"));
	
	fs::path base_path = fs::path(fileReferencePt->Attribute("fullpath")).lexically_normal();
	fs::path temp_path_ = fs::canonical(base_path, fs_error_code_);
	
	if (fs_error_code_.value() == 0 && DoesFileExist(temp_path_))
		return temp_path_;

	temp_path_.clear();
	temp_path_ = fs::absolute(base_path, fs_error_code_);
	if (fs_error_code_.value() == 0 && DoesFileExist(temp_path_))
		return temp_path_;
	
	if (DoesFileExist(base_path))
		return base_path;

	temp_path_.clear();	
	if (is_windows_platform_)
	{
		temp_path_ = "\\\\" + server_name + "\\" + server_volume_name;
		if (fileReferencePt->Attribute("target_is_folder", "0") && base_path.has_filename())
			temp_path_ /= base_path.filename().string().c_str();
		if (DoesFileExist(temp_path_))
			return temp_path_;
	}
	temp_path_.clear();

	if (base > 0 && target > 0)
	{
		fs::path temp_path_ = aepxXmlDocumentPath;
		fs::path file_relative_base = base_path;

		auto it = file_relative_base.end();

		while (base-- && temp_path_.has_parent_path())
			temp_path_ = temp_path_.parent_path();
		while (it != file_relative_base.begin() && target--)
			--it;

		while (it != file_relative_base.end())
			temp_path_ /= (it++)->string();

		if (DoesFileExist(temp_path_))
			return temp_path_;
	}	
	ERROR_CATCH_END_CONSTRUCT2
	return base_path;
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
			
			if(AeFileNode::FileExtensionCheck(pt, ".c4d"))
			{
				if (GF_AEGP_Relinker::GFCopy_C4D_File(libC4dPointer, node->GetFileFullSourcePath(i), node->GetFileFullCopyPath(i), node->GetFileRelinkPath(), node->GetFileNodeUid()) == NoError)
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
int AeBatchRelinker::HexTo32Int(const char* start, unsigned int char_index)
{
	if (start) {
		return std::stoi(std::string(start + char_index, 8), nullptr, 16);
	}
	return 0;
}

} // namespace RenderBeamer
