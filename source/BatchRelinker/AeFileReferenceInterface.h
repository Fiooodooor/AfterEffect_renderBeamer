#pragma once

//#include "tinyXml2.h"
#include "AeFileReferenceInterfaceHelper.h"

#ifdef AE_OS_WIN
    #define PATHPLATFORM "Win"
#else
    #define PATHPLATFORM "MacPOSIX"
#endif

class FileReferenceInterface
{
public:	
	FileReferenceInterface(const std::string &theFilesUID, tinyxml2::XMLElement *theFileReferencePointer) : filesUID(theFilesUID), fileReferencePointer(theFileReferencePointer), mainFilesPath("") {}
	virtual ~FileReferenceInterface() {}

	unsigned long long GetFilesNumber()									{ return FileReferenceInterfaceHelper::GetUniqueFileNode(nodeId)->GetFilenamesNumber(); }
	void SetFilesReferencePointer(tinyxml2::XMLElement *pt) { fileReferencePointer = pt; }
	tinyxml2::XMLElement *GetFilesReferencePointer()		{ return fileReferencePointer; }

	void SetStringFilesUID(const std::string &theUid)		{ filesUID = theUid; }
	std::string GetStringFilesUID()							{ return filesUID; }

	void SetWstringFilesUID(const std::wstring &theUid)		{ filesUID = std::string(theUid.begin(), theUid.end()); }
	std::wstring GetWstringFilesUID()						{ return std::wstring(GetStringFilesUID().begin(), GetStringFilesUID().end()); }
	
	void SetMainFilesPath(fs::path &mainPath)				{ mainFilesPath = mainPath; }
	fs::path GetMainFilePath()								{ return mainFilesPath; }
	
	fs::path GetFullFilePath(long FileNumber=0) const		{ return FileReferenceInterfaceHelper::GetUniqueFileNode(nodeId)->GetFileFullCopyPath(FileNumber); }

	virtual bool RelinkFiles() = 0;
	
protected:
	std::string filesUID;
	tinyxml2::XMLElement *fileReferencePointer;
	fs::path mainFilesPath;
	long nodeId;
	
private:
	FileReferenceInterface() : filesUID("00000000"), fileReferencePointer(nullptr), mainFilesPath("") {}
};

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

class SingleFileReference : public FileReferenceInterface
{
public:
	SingleFileReference(const std::string &theFilesUID, tinyxml2::XMLElement *theFileReferencePointer)
			: FileReferenceInterface(theFilesUID, theFileReferencePointer) { }
	
	void AddFile(const fs::path &fileName);
	bool RelinkFiles() override;
};

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

class SequenceListFileReference : public FileReferenceInterface
{
public:
	SequenceListFileReference(const std::string &theFilesUID, tinyxml2::XMLElement *theFileReferencePointer)
			: FileReferenceInterface(theFilesUID, theFileReferencePointer) { }
	
	void AddFile(tinyxml2::XMLElement *fileReference);
	bool RelinkFiles() override;

};

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

class SequenceMaskFileReference : public FileReferenceInterface
{
public:
	SequenceMaskFileReference(const std::string &theFilesUID, tinyxml2::XMLElement *theFileReferencePointer)
			: FileReferenceInterface(theFilesUID, theFileReferencePointer)
			, filesMaskBase(nullptr), filesMaskExtension(nullptr) { }
	
	void AddFilesByMask(tinyxml2::XMLElement *base, tinyxml2::XMLElement *extension);
	bool RelinkFiles() override;

protected:
	tinyxml2::XMLElement *filesMaskBase;
	tinyxml2::XMLElement *filesMaskExtension;	
};
