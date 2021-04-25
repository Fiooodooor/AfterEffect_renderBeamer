#ifndef GFSRQNODEWRAPPER_HPP
#define GFSRQNODEWRAPPER_HPP

#include "../AeGfsFileCreatorStructures.h"
#include "../NodeObjects/AeConteiner.h"
#include "pico_json.h"

class gfs_rq_node_wrapper
{
public:
	static ErrorCodesAE serialize(AeSceneContainer &scene_items_container, std::string &out_serialized_buffer);
	static ErrorCodesAE gfs_node_serialize(gfsRqItem *in, std::string &out);
	static ErrorCodesAE gfs_node_outputs_serialize(std::vector<gfsRqItemOutput*> &in_list, std::string &out_buffer);

	static ErrorCodesAE deserialize(AeSceneContainer &scene_items_container, std::string &in_string_buffer);
	static ErrorCodesAE gfs_node_deserialize(const picojson::value::object &in_object, std::list<gfsRqItem*> &in_items_list);
	static ErrorCodesAE gfs_node_outputs_deserialize(const picojson::value::array& in_array, std::vector<gfsRqItemOutput*> &in_items_vector);
	//ErrorCodesAE gfs_node_deserialize(std::string &in, gfsRqItem *out);
private:
};


#endif