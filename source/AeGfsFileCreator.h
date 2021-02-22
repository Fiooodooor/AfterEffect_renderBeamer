#pragma once
#ifndef AEGFSFILEBUILDER_HPP
#define AEGFSFILEBUILDER_HPP

#include "AeGfsFileCreatorStructures.h"
#include "BatchRelinker/tinyXml2.h"

class AeGfsFileCreator
{
public:
	~AeGfsFileCreator();

	static AeGfsFileCreator* getInstance();
	ErrorCodesAE InitGfsFileBuilder(beamerParamsStruct const &theBps);
	ErrorCodesAE InitGfsFileCreator(fs::path const &gfsFilePath, fs::path const &fileName, fs::path const &outPath, fs::path const &fontsDir, A_char const *version);
	ErrorCodesAE ClearGfsFileCreator();
	ErrorCodesAE GenerateAndSaveDocument();

	ErrorCodesAE PushRenderQueueItem(gfsRqItem *node);
	ErrorCodesAE PushFontNode(gfsFontNode *node_pt);
	ErrorCodesAE PushEffectNode(gfsEffectNode *node_pt);

	ErrorCodesAE SetGfsFilePath(fs::path const &gfsFilePath);
	ErrorCodesAE SetOutFileName(fs::path const &fileName);
	ErrorCodesAE SetOutFilePath(fs::path const &outPath);
	ErrorCodesAE SetFontsDirPath(fs::path const &fontsDir);
	ErrorCodesAE SetAeVersionString(A_char const *version);

protected:
	AeGfsFileCreator();
	ErrorCodesAE SaveDocument();
	ErrorCodesAE GenerateRenderQueueItems();
	ErrorCodesAE GenerateFontItems();
	ErrorCodesAE GenerateEffectItems();

	fs::path outFileName;
	fs::path outFilePath;
	fs::path outFontsDir;
	std::string versionString;

	std::vector<gfsRqItem*> rqItems;
	std::vector<gfsEffectNode*> effectsList;
	std::vector<gfsFontNode*> fontsList;

private:
	fs::path gfs_file_path;
	tinyxml2::XMLDocument gfs_document;
	tinyxml2::XMLDeclaration *document_declaration;
	tinyxml2::XMLElement *doc_root, *doc_settings, *doc_aftereffects;
	tinyxml2::XMLElement *rq_item, *font_item, *effect_item;
};



#endif
