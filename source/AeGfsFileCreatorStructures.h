#pragma once
#ifndef AEGFSFILECREATORSTRUCTURES_HPP
#define AEGFSFILECREATORSTRUCTURES_HPP

#include "GF_GlobalTypes.h"
#include <vector>

namespace RenderBeamer {

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
	A_long rq_out_id;
	A_char file_ext[8];
	A_char file_ext_format[32];
	A_char video_encoder[32];
	A_char video_pixel_format[32];
	A_char video_profile[32];
	A_char video_bit_rate[32];
	
	A_Boolean is_out_file_sequence;
	A_Boolean is_out_file_multi_frame;

	A_Boolean audio_available_in_comp;
	A_Boolean audio_out_enabled;
	AEGP_SoundDataFormat soundFormat;

	fs::path outputFile;
} gfsRqItemOutput;
/* A_FpLong sample_rateF; AEGP_SoundEncoding encoding; A_long bytes_per_sampleL; A_long num_channelsL;// 1 for mono, 2 for stereo } AEGP_SoundDataFormat;*/

typedef struct gfsRqItem {
	A_long rq_id;
	A_long rq_out_modules_n;
	A_long renderable;
	A_long composition_id;
	A_long width;
	A_long height;
	std::string name;
	A_char frame_range[AEGP_MAX_ITEM_NAME_SIZE];
	A_char fps[AEGP_MAX_ITEM_NAME_SIZE];
	std::vector<gfsRqItemOutput*> output_mods;
} gfsRqItem;

} // namespace RenderBeamer

#endif
/*
A_long renderable;
A_long rq_id;
A_long composition_id;
A_long width;
A_long height;
std::string name;
A_char frame_range[AEGP_MAX_ITEM_NAME_SIZE];
A_char fps[AEGP_MAX_ITEM_NAME_SIZE];
 *
 *
 * 
 */
