#pragma once
#ifndef AEGFSFILECREATORSTRUCTURES_HPP
#define AEGFSFILECREATORSTRUCTURES_HPP

#include "GF_GlobalTypes.h"
#include <vector>

typedef struct gfsFontNode {
	unsigned long long subFontNr;
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
	A_char file_ext[8];
	A_char file_ext_format[32];
	A_char video_encoder[32];
	A_char video_pixel_format[32];
	A_char video_profile[32];
	A_char video_bitrate[32];
	
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
	std::string composition_name;
	A_long width;
	A_long height;
	A_Boolean continue_on_missing;
	A_Boolean smart_collect;
	A_char frame_string[AEGP_MAX_ITEM_NAME_SIZE];
	A_char fps[AEGP_MAX_ITEM_NAME_SIZE];
	std::vector<gfsRqItemOutput*> output_mods;
} gfsRqItem;

#endif