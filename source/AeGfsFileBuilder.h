#pragma once
#ifndef AEGFSFILEBUILDER_HPP
#define AEGFSFILEBUILDER_HPP

#include "GF_GlobalTypes.h"
#include <vector>
#include "NodeObjects/AeCompNode.h"
#define PBUFFER_SIZE 4096

typedef struct tagGFSFONTNODE {
	int subFontNr;
	std::string fontName;
	fs::path fontFile;
} gfsFontNode;

typedef struct tagGFSEFFECTNODE {
	A_long installKey;
	std::string safeName;
	std::string matchName;
	std::string effectCategory;
} gfsEffectNode;

typedef struct tagGFFRQITEMOUTPUT {
	A_long indexNr;
	A_char outputType[48];
	A_char outputInfo[96];

	A_Boolean outFileIsSeq;
	A_Boolean outFileIsMultiframe;

	A_Boolean outputAudioEnabled;
	A_Boolean outputAudioSetToUse;
	AEGP_SoundDataFormat soundFormat;

	fs::path outputFile;
	
} gfsRqItemOutput;
/* A_FpLong sample_rateF; AEGP_SoundEncoding encoding; A_long bytes_per_sampleL; A_long num_channelsL;// 1 for mono, 2 for stereo } AEGP_SoundDataFormat;*/

typedef struct tagGFFRQITEM {
	A_long indexNr;
	A_char compositioName[AEGP_MAX_ITEM_NAME_SIZE+4];
	A_long width;
	A_long height;
	A_char frameString[36];
	A_char fps[36];
	std::vector<gfsRqItemOutput*> outMods;
} gfsRqItem;

class AeGfsFileBuilder
{
public:
	static AeGfsFileBuilder* getInstance();
	~AeGfsFileBuilder();

	ErrorCodesAE initGfsFileBuilder(beamerParamsStruct const &theBps);
	ErrorCodesAE initGfsFileBuilder(fs::path const &gfsFilePath, fs::path const &fileName, fs::path const &outPath, fs::path const &fontsDir, A_char const *version);

	ErrorCodesAE generateFullGfs();
	ErrorCodesAE generateGfsAndXmlHeader();
	ErrorCodesAE generateSetting();
	ErrorCodesAE generateRqItems();
	ErrorCodesAE generateFonts();
	ErrorCodesAE generateEffects();
	ErrorCodesAE generateTail();

	ErrorCodesAE setGfsFilePath(fs::path const &gfsFilePath);
	ErrorCodesAE setOutFileName(fs::path const &fileName);
	ErrorCodesAE setOutFilePath(fs::path const &outPath);
	ErrorCodesAE setFontsDirPath(fs::path const &fontsDir);
	ErrorCodesAE setAeVersionString(A_char const *version);

	ErrorCodesAE pushRqItem(gfsRqItem *node);

	ErrorCodesAE pushFontNode(gfsFontNode* node_pt);
	ErrorCodesAE pushEffectsNode(gfsEffectNode* node_pt);
    
protected:
	AeGfsFileBuilder();

	fs::path outFileName;
	fs::path outFilePath;
	fs::path outFontsDir;
	A_char versionString[36];

	std::vector<gfsRqItem*> rqItems;
	std::vector<gfsEffectNode*> effectsList;
	std::vector<gfsFontNode*> fontsList;
	wchar_t pbuff[PBUFFER_SIZE];
private:
	std::ofstream f_prv;
	fs::path f_path;
	std::locale toUtfLocale;
};



#endif
