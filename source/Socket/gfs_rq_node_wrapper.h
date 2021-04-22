#ifndef GFSRQNODEWRAPPER_HPP
#define GFSRQNODEWRAPPER_HPP

#include "../AeGfsFileCreatorStructures.h"
#include "../NodeObjects/AeConteiner.h"

class gfs_rq_node_wrapper
{
public:
	static ErrorCodesAE serialize(AeSceneConteiner &scene_items_container, std::string &out_serialized_buffer);
	static ErrorCodesAE gfs_node_serialize(gfsRqItem *in, std::string &out);
	static ErrorCodesAE gfs_node_outputs_serialize(std::vector<gfsRqItemOutput*> &in_list, std::string &out_buffer);
	//ErrorCodesAE gfs_node_deserialize(std::string &in, gfsRqItem *out);
private:
};


#endif