#pragma once

//#include "tinyXml2.h"
#include "AeFileNode.h"

#ifdef AE_OS_WIN
    #define PATHPLATFORM "Win"
#else
    #define PATHPLATFORM "MacPOSIX"
#endif

typedef unsigned long long AeFileNodeID;

class FileReferenceInterface
{
public:
	FileReferenceInterface(std::string theFilesUID, tinyxml2::XMLElement* theFileReferencePointer);
	//virtual ~FileReferenceInterface() {}

	void SetFilesReferencePointer(tinyxml2::XMLElement *pt) { fileReferencePointer = pt; }
	tinyxml2::XMLElement *GetFilesReferencePointer() const	{ return fileReferencePointer; }

	void SetStringFilesUID(const std::string &theUid)		{ filesUID = theUid; }
	std::string GetStringFilesUID() const					{ return filesUID; }

	void SetWstringFilesUID(const std::wstring &theUid)		{ filesUID = std::string(theUid.begin(), theUid.end()); }
	std::wstring GetWstringFilesUID() const					{ return std::wstring(GetStringFilesUID().begin(), GetStringFilesUID().end()); }

	void SetMainFilesPath(fs::path& mainPath);
	fs::path GetMainFilePath() const;

	void SetNodeId(const AeFileNodeID id)					{ nodeId = id; }
	AeFileNodeID GetNodeId() const							{ return nodeId; }

	virtual AeFileNode* AddFiles() = 0;
	virtual bool RelinkFiles(AeFileNode* node) = 0;
	
protected:
	std::string filesUID;
	tinyxml2::XMLElement *fileReferencePointer;
	fs::path mainFilesPath;
	unsigned long long nodeId;
	
private:
	FileReferenceInterface() : filesUID("00000000"), fileReferencePointer(nullptr), mainFilesPath(""), nodeId(0) {}
};

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

class SingleFileReference : public FileReferenceInterface
{
public:
	SingleFileReference(const std::string& theFilesUID, tinyxml2::XMLElement* theFileReferencePointer, fs::path fileName);

	AeFileNode* AddFiles() override;
	bool RelinkFiles(AeFileNode* node) override;

private:
	fs::path file_name;
};

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

class SequenceListFileReference : public FileReferenceInterface
{
public:
	SequenceListFileReference(const std::string& theFilesUID, tinyxml2::XMLElement* theFileReferencePointer, tinyxml2::XMLElement *fileReference);

	AeFileNode* AddFiles() override;
	bool RelinkFiles(AeFileNode* node) override;

private:
	tinyxml2::XMLElement *file_reference;
};

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

class SequenceMaskFileReference : public FileReferenceInterface
{
public:
	SequenceMaskFileReference(const std::string& theFilesUID, tinyxml2::XMLElement* theFileReferencePointer, tinyxml2::XMLElement *base, tinyxml2::XMLElement *extension);

	AeFileNode* AddFiles() override;
	bool RelinkFiles(AeFileNode* node) override;

protected:
	tinyxml2::XMLElement *files_mask_base;
	tinyxml2::XMLElement *files_mask_extension;
};
