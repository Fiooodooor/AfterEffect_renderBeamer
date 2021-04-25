#include "gfs_rq_node_wrapper.h"

#define NUMBER_FIELD(  FIELD, VALUE ) std::string("\\\"" #FIELD "\\\":\\\"") + std::to_string( VALUE ) + "\\\""
#define CHAR_FIELD(  FIELD, VALUE ) std::string("\\\"" #FIELD "\\\":\\\"") + std::string( VALUE ) + "\\\""
#define STRING_FIELD(  FIELD, VALUE ) std::string("\\\"" #FIELD "\\\":\\\"") + ( VALUE ) + "\\\""

ErrorCodesAE gfs_rq_node_wrapper::serialize(AeSceneContainer &scene_items_container, std::string &out_serialized_buffer)
{
	ERROR_CATCH_START
	if (scene_items_container.gfsRqItemsList.empty())
		return NullResult;
	
	auto counter = scene_items_container.gfsRqItemsList.size();
	out_serialized_buffer.reserve(16000);
	out_serialized_buffer = "SCRIPT=initRenderbeamerPanel( '{" + NUMBER_FIELD( ignore_missings, scene_items_container.ignore_missings_assets );
	out_serialized_buffer += ',' + NUMBER_FIELD( smart_collect, scene_items_container.smart_collect );
	out_serialized_buffer += R"(,\"data\":[)";
	for (auto *it : scene_items_container.gfsRqItemsList) {
		gfs_node_serialize(it, out_serialized_buffer);
		if (--counter) out_serialized_buffer += ',';
	}
	out_serialized_buffer += "]} ');\n";

	ERROR_CATCH_END_NO_INFO_RETURN
}

ErrorCodesAE gfs_rq_node_wrapper::gfs_node_serialize(gfsRqItem *in, std::string &out)
{
	if (!in)
		return NullPointerResult;

	std::string gfs_out_buffer;
	
	out += '{' + NUMBER_FIELD( rq_id, in->rq_id);
	out += ',' + NUMBER_FIELD( rq_out_modules_n, in->rq_out_modules_n);
	out += ',' + NUMBER_FIELD( renderable, in->renderable);
	out += ',' + NUMBER_FIELD( composition_id, in->composition_id);
	out += ',' + NUMBER_FIELD( width, in->width);
	out += ',' + NUMBER_FIELD( height, in->height);
	out += ',' + STRING_FIELD( name, in->name);
	out += ',' + CHAR_FIELD( frame_range, in->frame_range);
	out += ',' + CHAR_FIELD( fps, in->fps);
	out += R"(, \"out_modules\":[)";
	if (gfs_node_outputs_serialize(in->output_mods, gfs_out_buffer) == NoError)
		out += gfs_out_buffer;
	out += "]}" ;
	
	return NoError;
}

ErrorCodesAE gfs_rq_node_wrapper::gfs_node_outputs_serialize(std::vector<gfsRqItemOutput*> &in_list, std::string &out_buffer)
{
	const int it = 0;
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


ErrorCodesAE gfs_rq_node_wrapper::deserialize(AeSceneContainer &scene_items_container, std::string &in_string_buffer)
{
	ERROR_CATCH_START
	picojson::value main_value;
	
	const auto conversion_err = picojson::parse(main_value, in_string_buffer.c_str()+5);
	if (!conversion_err.empty()) {
		return ErrorResult;
	}
	if (!main_value.is<picojson::object>()) {
		return NullResult;	// add parsing_error
	}

	const auto& parsed_obj = main_value.get<picojson::object>();
	for (auto i = parsed_obj.begin(); i != parsed_obj.end(); ++i)
	{
		if (i->first == "data")
		{
			if (i->second.is<picojson::array>())
			{
				const auto& rq_items_array = i->second.get<picojson::array>();
				for (auto rq_item : rq_items_array)
				{
					gfs_node_deserialize(rq_item.get<picojson::object>(), scene_items_container.gfsRqItemsList);
				}
			}
		}
		else if (i->first == "ignore_missings")
		{
			scene_items_container.ignore_missings_assets = static_cast<A_Boolean>(strtol(i->second.to_str().c_str(), nullptr, 10));
		}
		else if (i->first == "smart_collect")
		{
			scene_items_container.smart_collect = static_cast<A_Boolean>(strtol(i->second.to_str().c_str(), nullptr, 10));
		}
	}
	ERROR_CATCH_END_NO_INFO_RETURN
}
ErrorCodesAE gfs_rq_node_wrapper::gfs_node_deserialize(const picojson::value::object &in_object, std::list<gfsRqItem*> &in_items_list)
{
	ERROR_CATCH_START
		const auto rq_id_key = in_object.find("rq_id");
	if (rq_id_key != in_object.end())
	{
		gfsRqItem* node = nullptr;
		const auto rq_id = strtol(rq_id_key->second.to_str().c_str(), nullptr, 10);

		for (auto *it : in_items_list)
		{
			if (it->rq_id == rq_id)
			{
				node = it;
				break;
			}
		}
		if (node != nullptr)
		{
			for (auto i = in_object.begin(); i != in_object.end(); ++i)
			{
				if (i->first == "renderable") {
					node->renderable = static_cast<int>(strtol(i->second.to_str().c_str(), nullptr, 10));
				}
				else if (i->first == "width") {
					node->width = static_cast<int>(strtol(i->second.to_str().c_str(), nullptr, 10));
				}
				else if (i->first == "height") {
					node->height = static_cast<int>(strtol(i->second.to_str().c_str(), nullptr, 10));
				}
				else if (i->first == "frame_range") {
					ASTRNCPY(node->frame_range, i->second.to_str().c_str(), sizeof(gfsRqItem::frame_range) - 1)
				}
				else if (i->first == "fps") {
					ASTRNCPY(node->fps, i->second.to_str().c_str(), sizeof(gfsRqItem::fps) - 1)
				}
				else if (i->first == "out_modules") {
					gfs_node_outputs_deserialize(i->second.get<picojson::array>(), node->output_mods);
				}
			}
		}
	}
	else
	{
		//std::cout << "error not found! " << std::endl;
	}

	ERROR_CATCH_END_NO_INFO_RETURN
}
ErrorCodesAE gfs_rq_node_wrapper::gfs_node_outputs_deserialize(const picojson::value::array& in_array, std::vector<gfsRqItemOutput*> &in_items_vector)
{
	ERROR_CATCH_START
		for (auto out_it : in_array)
		{
			auto rq_out = out_it.get<picojson::object>();
			const auto rq_out_id_key = rq_out.find("rq_out_id");
			if (rq_out_id_key != rq_out.end())
			{
				gfsRqItemOutput* node = nullptr;
				const auto rq_out_id = strtol(rq_out_id_key->second.to_str().c_str(), nullptr, 10);

				for (auto *it : in_items_vector)
				{
					if (it->rq_out_id == rq_out_id)
					{
						node = it;
						break;
					}
				}
				if (node != nullptr)
				{
					for (auto pt = rq_out.begin(); pt != rq_out.end(); ++pt)
					{
						if (pt->first == "is_out_file_sequence") {
							node->is_out_file_sequence = static_cast<A_Boolean>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
						else if (pt->first == "is_out_file_multi_frame") {
							node->is_out_file_multi_frame = static_cast<A_Boolean>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
						else if (pt->first == "file_ext") {
							ASTRNCPY(node->file_ext, pt->second.to_str().c_str(), sizeof(gfsRqItemOutput::file_ext) - 1)
						}
						else if (pt->first == "file_ext_format") {
							ASTRNCPY(node->file_ext_format, pt->second.to_str().c_str(), sizeof(gfsRqItemOutput::file_ext_format) - 1)
						}
						else if (pt->first == "video_encoder") {
							ASTRNCPY(node->video_encoder, pt->second.to_str().c_str(), sizeof(gfsRqItemOutput::video_encoder) - 1)
						}
						else if (pt->first == "video_pixel_format") {
							ASTRNCPY(node->video_pixel_format, pt->second.to_str().c_str(), sizeof(gfsRqItemOutput::video_pixel_format) - 1)
						}
						else if (pt->first == "video_profile") {
							ASTRNCPY(node->video_profile, pt->second.to_str().c_str(), sizeof(gfsRqItemOutput::video_profile) - 1)
						}
						else if (pt->first == "video_bit_rate") {
							ASTRNCPY(node->video_bit_rate, pt->second.to_str().c_str(), sizeof(gfsRqItemOutput::video_bit_rate) - 1)
						}
						else if (pt->first == "audio_available_in_comp") {
							node->audio_available_in_comp = static_cast<A_Boolean>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
						else if (pt->first == "audio_out_enabled") {
							node->audio_out_enabled = static_cast<A_Boolean>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
						else if (pt->first == "audio_sample_rate") {
							node->soundFormat.sample_rateF = strtol(pt->second.to_str().c_str(), nullptr, 10);
						}
						else if (pt->first == "audio_aegp_encoding") {
							node->soundFormat.encoding = static_cast<int>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
						else if (pt->first == "audio_depth") {
							node->soundFormat.bytes_per_sampleL = static_cast<int>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
						else if (pt->first == "audio_channels") {
							node->soundFormat.num_channelsL = static_cast<int>(strtol(pt->second.to_str().c_str(), nullptr, 10));
						}
					}
				}
			}
		}
	ERROR_CATCH_END_NO_INFO_RETURN
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
