#pragma once

#include "AeFileReferenceInterface.h"
#include "../GF_AEGP_Relinker.h"

class AeBatchRelinker
{
public:
	AeBatchRelinker(SPBasicSuite *pb, PlatformLibLoader* c4dLoader, rbProjectClass& rbLogger, PF_AppProgressDialogP &progressD, const fs::path &aepxPath, const fs::path &aepxRemote)
		: picaBasic(pb), libC4dPointer(c4dLoader), rbProjLogger(&rbLogger), progressDialog(progressD), aepxXmlDocumentPath(aepxPath), aepxXmlDocumentRemotePath(aepxRemote) {}
	
	ErrorCodesAE ParseAepxXmlDocument();
	ErrorCodesAE CopyAndRelinkFiles(const fs::path &localAssetsPath, const fs::path &remoteAssetsPath);
	ErrorCodesAE CopyUniqueFiles(const fs::path &localCopyPath, const fs::path &remoteCopyPath);
	
	FileReferenceInterface *CreateFileReference(tinyxml2::XMLElement *fileReferencePt);

protected:
	SPBasicSuite *picaBasic;	
	PlatformLibLoader* libC4dPointer;
	rbProjectClass *rbProjLogger;
	PF_AppProgressDialogP progressDialog;
	
	tinyxml2::XMLDocument aepxXmlDocument;
	fs::path aepxXmlDocumentPath;
	fs::path aepxXmlDocumentRemotePath;
	std::vector<FileReferenceInterface*> fileItemNodes;
};

