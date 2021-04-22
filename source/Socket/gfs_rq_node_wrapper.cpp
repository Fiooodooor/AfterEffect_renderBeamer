#include "gfs_rq_node_wrapper.h"

#define NUMBER_FIELD(  FIELD, VALUE ) std::string("\\\"" #FIELD "\\\":\\\"") + std::to_string( VALUE ) + "\\\""
#define CHAR_FIELD(  FIELD, VALUE ) std::string("\\\"" #FIELD "\\\":\\\"") + std::string( VALUE ) + "\\\""
#define STRING_FIELD(  FIELD, VALUE ) std::string("\\\"" #FIELD "\\\":\\\"") + ( VALUE ) + "\\\""



ErrorCodesAE gfs_rq_node_wrapper::serialize(AeSceneConteiner &scene_items_container, std::string &out_serialized_buffer)
{
	out_serialized_buffer.reserve(16000);
	out_serialized_buffer = "SCRIPT=initRenderbeamerPanel( '{" + NUMBER_FIELD( ignore_missings, 1 );
	out_serialized_buffer += ',' + NUMBER_FIELD( smart_collect, 0 );
	out_serialized_buffer += ",\\\"data\\\":[";
	gfs_node_serialize(scene_items_container.gfsRqItemsList.back(), out_serialized_buffer);
	out_serialized_buffer += "]} ');\n";

	return NoError;
}

ErrorCodesAE gfs_rq_node_wrapper::gfs_node_serialize(gfsRqItem *in, std::string &out)
{
	if (!in)
		return NullPointerResult;

	std::string gfs_out_buffer;
	gfs_out_buffer.reserve(4096);
	
	out += '{' + NUMBER_FIELD( rq_id, in->rq_id);
	out += ',' + NUMBER_FIELD( rq_out_modules_n, in->rq_out_modules_n);
	out += ',' + NUMBER_FIELD( renderable, in->renderable);
	out += ',' + NUMBER_FIELD( composition_id, in->composition_id);
	out += ',' + NUMBER_FIELD( width, in->width);
	out += ',' + NUMBER_FIELD( height, in->height);
	out += ',' + STRING_FIELD( name, in->name);
	out += ',' + CHAR_FIELD( frame_range, in->frame_range);
	out += ',' + CHAR_FIELD( fps, in->fps);
	out += ", \\\"out_modules\\\":[";
	if (gfs_node_outputs_serialize(in->output_mods, gfs_out_buffer) == NoError)
		out += gfs_out_buffer;
	out += "]}" ;
	
	return NoError;
}

ErrorCodesAE gfs_rq_node_wrapper::gfs_node_outputs_serialize(std::vector<gfsRqItemOutput*> &in_list, std::string &out_buffer)
{
	int it = 0;
	gfsRqItemOutput *in = nullptr;
	
	if (in_list.empty())
		return NoRqItemOutputs;

	if (!in_list.at(it))
		return NullPointerResult;
	
	in = in_list.at(it);
	
	out_buffer.reserve(4096);
	out_buffer = '{' + NUMBER_FIELD(rq_out_id, in->rq_out_id);
	out_buffer += ',' + NUMBER_FIELD(is_out_file_sequence, in->is_out_file_sequence);
	out_buffer += ',' + NUMBER_FIELD(is_out_file_multi_frame, in->is_out_file_multi_frame);
	out_buffer += ',' + CHAR_FIELD(file_ext, in->file_ext);
	out_buffer += ',' + CHAR_FIELD(file_ext_format, in->file_ext_format);
	out_buffer += ',' + CHAR_FIELD(video_encoder, in->video_encoder);
	out_buffer += ',' + CHAR_FIELD(video_pixel_format, in->video_pixel_format);
	out_buffer += ',' + CHAR_FIELD(video_profile, in->video_profile);
	out_buffer += ',' + CHAR_FIELD(video_bit_rate, in->video_bit_rate);
	out_buffer += ',' + NUMBER_FIELD(audio_available_in_comp, in->audio_available_in_comp);
	out_buffer += ',' + NUMBER_FIELD(audio_out_enabled, in->audio_out_enabled);
	out_buffer += ',' + NUMBER_FIELD(audio_sample_rate, static_cast<long>( in->soundFormat.sample_rateF ));
	out_buffer += ',' + NUMBER_FIELD(audio_aegp_encoding, in->soundFormat.encoding);
	out_buffer += ',' + NUMBER_FIELD(audio_depth, in->soundFormat.bytes_per_sampleL);
	out_buffer += ',' + NUMBER_FIELD(audio_channels, in->soundFormat.num_channelsL);
	out_buffer += '}';

	return NoError;
}
/*
A_long rq_id;										<=>		rq_id = "1";
A_long rq_out_modules_n;							<=>		rq_out_modules_n = "1";
A_long renderable;									<=>		renderable = "0";
A_long composition_id;								<=>		composition_id = "0";
A_long width;										<=>		width = "0";
A_long height;										<=>		height = "0";
std::string name;									<=>		name = "Composition_Name";
A_char frame_range[AEGP_MAX_ITEM_NAME_SIZE];		<=>		frame_range = "1to10s1";
A_char fps[AEGP_MAX_ITEM_NAME_SIZE];				<=>		fps = "29.97";
std::vector<gfsRqItemOutput*> output_mods;			<=>		out_modules = [array]
*/

/* 
A_long rq_out_id;								<=>		rq_out_id = "0";
A_Boolean is_out_file_sequence;					<=>		is_out_file_sequence = "1";
A_Boolean is_out_file_multi_frame;				<=>		is_out_file_multi_frame = "0";
A_char file_ext[8];								<=>		file_ext = "png";
A_char file_ext_format[32];						<=>		file_ext_format = "png 16bit";
A_char video_encoder[32];						<=>		video_encoder = "";
A_char video_pixel_format[32];					<=>		video_pixel_format = "";
A_char video_profile[32];						<=>		video_profile = "";
A_char video_bit_rate[32];						<=>		video_bit_rate = "5000";
A_Boolean audio_available_in_comp;				<=>		audio_available_in_comp = "0";
A_Boolean audio_out_enabled;					<=>		audio_out_enabled = "0";
AEGP_SoundDataFormat soundFormat;
	A_FpLong			sample_rateF;			<=>		audio_sample_rate = "48000";
	AEGP_SoundEncoding	encoding;				<=>		audio_aegp_encoding = "3"
	A_long				bytes_per_sampleL;		<=>		audio_depth = "4";
	A_long				num_channelsL;			<=>		audio_channels = "2";
fs::path outputFile;							<=>
*/