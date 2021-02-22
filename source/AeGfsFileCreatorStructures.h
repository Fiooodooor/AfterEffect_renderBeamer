#pragma once
#ifndef AEGFSFILECREATORSTRUCTURES_HPP
#define AEGFSFILECREATORSTRUCTURES_HPP

#include "GF_GlobalTypes.h"
#include <vector>

typedef struct gfsFontNode {
	int subFontNr;
	std::string fontName;
	fs::path fontFile;
} gfsFontNode;

typedef struct gfsEffectNode {
	A_long installKey;
	std::string effectName;
	std::string matchName;
	std::string effectCategory;
} gfsEffectNode;

typedef struct gfsRqItemOutput {
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

typedef struct gfsRqItem {
	A_long indexNr;
	A_char compositioName[AEGP_MAX_ITEM_NAME_SIZE + 4];
	A_long width;
	A_long height;
	A_char frameString[36];
	A_char fps[36];
	std::vector<gfsRqItemOutput*> outMods;
} gfsRqItem;

#endif