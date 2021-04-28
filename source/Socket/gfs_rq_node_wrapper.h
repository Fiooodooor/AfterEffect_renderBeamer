#ifndef GFSRQNODEWRAPPER_HPP
#define GFSRQNODEWRAPPER_HPP

#include "../AeGfsFileCreatorStructures.h"
#include "../NodeObjects/AeConteiner.h"

namespace RenderBeamer {

class gfs_rq_node_wrapper
{
public:
    static ErrorCodesAE serialize(AeSceneContainer &scene_items_container, std::string &out_serialized_buffer);
    static ErrorCodesAE gfs_node_serialize(gfsRqItem *in, std::string &out);
    static ErrorCodesAE gfs_node_outputs_serialize(std::vector<gfsRqItemOutput*> &in_list, std::string &out_buffer);

    static ErrorCodesAE deserialize(AeSceneContainer &scene_items_container, std::string &in_string_buffer);
    static ErrorCodesAE gfs_node_deserialize(void *in_object, std::list<gfsRqItem*> &in_items_list);
    static ErrorCodesAE gfs_node_outputs_deserialize(void *in_array, std::vector<gfsRqItemOutput*> &in_items_vector);
    
};

} //namespace RenderBeamer
#endif
