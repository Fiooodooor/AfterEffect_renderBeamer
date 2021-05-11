#ifndef AEBATCHRELINKER_HPP
#define AEBATCHRELINKER_HPP

#include "AeFileReferenceInterface.h"
#include "../GF_AEGP_Relinker.h"
namespace RenderBeamer {

class AeBatchRelinker
{
public:
    AeBatchRelinker(SPBasicSuite* pb, PlatformLibLoader* c4dLoader, rbProjectClass* rbLogger, PF_AppProgressDialogP* progressD, const fs::path& aepxPath);
	~AeBatchRelinker();
	
	ErrorCodesAE ParseAepxXmlDocument();
	ErrorCodesAE CopyAndRelinkFiles(const fs::path &localAssetsPath, const fs::path &remoteAssetsPath);
	ErrorCodesAE CopyUniqueFiles(const fs::path &localCopyPath, const fs::path &remoteCopyPath);
	
	FileReferenceInterface *CreateFileReference(tinyxml2::XMLElement *fileReferencePt);

protected:
	AeFileNodeID PushUniqueFilePath(AeFileNode *node);
	AeFileNode *GetUniqueFileNode(AeFileNodeID nodeNr) const;
	unsigned long long GetUniqueFilesTotalSize() const;
	A_long GetUniqueFilesTotalSizeA() const;
	void ListUniqueFiles() const;
	long GetLongFilesUID(std::string uid) const;
	
	SPBasicSuite *picaBasic;	
	PlatformLibLoader* libC4dPointer;
	rbProjectClass *rbProjLogger;
	PF_AppProgressDialogP *progressDialog;
	
	tinyxml2::XMLDocument aepxXmlDocument;
	fs::path aepxXmlDocumentPath;
	std::vector<FileReferenceInterface*> fileItemNodes;

	unsigned long long unique_files_total_size;
	std::vector<AeFileNode*> unique_file_nodes;
};

}
#endif
