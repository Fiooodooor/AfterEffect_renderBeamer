#include "AeBatchRelinker.h"
#include <clocale>

ErrorCodesAE AeBatchRelinker::ParseAepxXmlDocument()
{
	AEGP_SuiteHandler suites(picaBasic);
	std::setlocale(LC_ALL, "en_US.utf8");
	FileReferenceInterfaceHelper::GetUniqueFilesTotalSize() = 0;
	try {
		if (aepxXmlDocument.LoadFile(aepxXmlDocumentPath.string().c_str()) != tinyxml2::XML_NO_ERROR)
			return ErrorResult;

		tinyxml2::XMLElement *AepxXmlElement = aepxXmlDocument.FirstChildElement();
		FileReferenceInterface *fileReference;
		
		while (AepxXmlElement)
		{
			if (AepxXmlElement->Value() && !std::string(AepxXmlElement->Value()).compare("fileReference"))
			{
				if (AepxXmlElement->Parent() && AepxXmlElement->Parent()->Parent() && !std::string(AepxXmlElement->Parent()->Parent()->Value()).compare("Pin"))
				{
					GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(progressDialog, 0, static_cast<A_long>(FileReferenceInterfaceHelper::GetUniqueFilesTotalSize())))
					rbProjLogger->logg("BatchAepxParser", "Path", AepxXmlElement->Attribute("fullpath"));
					fileReference = CreateFileReference(AepxXmlElement);					

					if (fileReference != nullptr)
						fileItemNodes.push_back(fileReference);

					GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(progressDialog, 0, static_cast<A_long>(FileReferenceInterfaceHelper::GetUniqueFilesTotalSize())))
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
			GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(progressDialog, 0, static_cast<A_long>(FileReferenceInterfaceHelper::GetUniqueFilesTotalSize())))
		}
	}
	catch (...) {
		return ErrorResult;
	}
	return NoError;
}

FileReferenceInterface *AeBatchRelinker::CreateFileReference(tinyxml2::XMLElement *fileReferencePt)
{
	FileReferenceInterface *fileRef = nullptr;
	std::string fileUid = "00000000";
	FS_ERROR_CODE(fileRefError)
    FS_ERROR_ASSIGN(fileRefError,100)

	try {		
		fs::path FileBasePath = fs::absolute(fs::path(fileReferencePt->Attribute("fullpath"))).lexically_normal();
		int ascendcount_base = fileReferencePt->IntAttribute("ascendcount_base");
		int ascendcount_target = fileReferencePt->IntAttribute("ascendcount_target");

		try {			
			fs::file_status FileBasePathStatus = status(FileBasePath);
			if (fs::status_known(FileBasePathStatus) ? !fs::exists(FileBasePathStatus) : !fs::exists(FileBasePath))
				throw fs::filesystem_error("File does not exist! Trying to AE style path resolve ", fileRefError);
			if (FileBasePathStatus.type() == FS_TYPE_UNKNOWN)
				return nullptr;
			if (fs::is_symlink(FileBasePath))
				FileBasePath = fs::read_symlink(FileBasePath);
		}
		catch(fs::filesystem_error &e) {
			rbProjLogger->loggErr("BatchAepxParser", "FullPath", e.what());			
			if (ascendcount_base > 0 && ascendcount_target > 0)
			{
				FileBasePath = aepxXmlDocumentPath;
				fs::path FileBaseRelative = fs::absolute(fs::path(fileReferencePt->Attribute("fullpath"))).lexically_normal();

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
				fileUid = std::to_string(FileReferenceInterfaceHelper::GetLongFilesUID(std::string(fileReferencePt->Parent()->Parent()->Parent()->FirstChildElement()->Attribute("bdata"))));	
			}	
		}

		if (fileReferencePt->Attribute("target_is_folder", "0"))
		{
			if (FileBasePath.has_filename())
			{
				fileRef = new SingleFileReference(fileUid, fileReferencePt);
				dynamic_cast<SingleFileReference*>(fileRef)->AddFile(FileBasePath);
			}
		}
		else
		{
			tinyxml2::XMLElement *fileReferenceSequence = fileReferencePt->Parent()->NextSiblingElement();
			FileBasePath += fs::path::preferred_separator;

			if (fileReferenceSequence)
			{
				if (!std::string(fileReferenceSequence->Value()).compare("StVc"))
				{
					fileReferenceSequence = fileReferenceSequence->FirstChildElement();
					if (fileReferenceSequence && !std::string(fileReferenceSequence->Value()).compare("StVS"))
					{
						fileRef = new SequenceListFileReference(fileUid, fileReferencePt);
						fileRef->SetMainFilesPath(FileBasePath);
						dynamic_cast<SequenceListFileReference*>(fileRef)->AddFile(fileReferenceSequence);				
					}
				}
				else if (!std::string(fileReferenceSequence->Value()).compare("string") && fileReferenceSequence->NextSiblingElement() && !std::string(fileReferenceSequence->NextSiblingElement()->Value()).compare("string"))
				{
					fileRef = new SequenceMaskFileReference(fileUid, fileReferencePt);
					fileRef->SetMainFilesPath(FileBasePath);
					dynamic_cast<SequenceMaskFileReference*>(fileRef)->AddFilesByMask(fileReferenceSequence, fileReferenceSequence->NextSiblingElement());
				}
			}
		}
	}
	catch (...) {
		return nullptr;
	}
	return fileRef;
}

ErrorCodesAE AeBatchRelinker::CopyAndRelinkFiles(const fs::path &localAssetsPath, const fs::path &remoteAssetsPath)
{
	ErrorCodesAE _error = CopyUniqueFiles(localAssetsPath.lexically_normal(), remoteAssetsPath.lexically_normal());
	if (_error == NoError)
	{
		for (auto pt : fileItemNodes)
			pt->RelinkFiles();

		_error = aepxXmlDocument.SaveFile(aepxXmlDocumentPath.string().c_str()) == tinyxml2::XML_NO_ERROR ? NoError : ErrorResult;
	}
	return _error;
}

ErrorCodesAE AeBatchRelinker::CopyUniqueFiles(const fs::path &localCopyPath, const fs::path &remoteCopyPath)
{
	AEGP_SuiteHandler suites(picaBasic);
	FS_ERROR_CODE(copyError)
	std::string relinkedFileName;
	unsigned long long i, relinkedFilesSize = 0, totalFilesSize = (FileReferenceInterfaceHelper::GetUniqueFilesTotalSize() >> 10);
	
	for (auto *node : FileReferenceInterfaceHelper::GetUniqueFilesContainer())
	{
		node->SetFileCopyPath(localCopyPath);
		node->SetFileRelinkPath(remoteCopyPath);
		
		for(i=0; i < node->GetFilenamesNumber(); ++i)
		{
			auto pt = fs::path(node->GetFilenameCouple(i)->sourceFileName);
			GF_PROGRESS(suites.AppSuite6()->PF_AppProgressDialogUpdate(progressDialog, static_cast<A_long>(relinkedFilesSize), static_cast<A_long>(totalFilesSize)))
			
			if(pt.has_extension() && pt.extension().compare(fs::path(".c4d")) == 0)
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
