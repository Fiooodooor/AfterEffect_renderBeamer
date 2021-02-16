#pragma once
#ifndef AEOBJECTNODE_H
#define AEOBJECTNODE_H

#include <list>
#include "../GF_GlobalTypes.h"

class ObjectCount 
{
	static int count;
protected:
	ObjectCount();

public:
	int static showCount();
};

class AePathNode
{
public:
	AePathNode() {};

	std::wstring oryginalPath;
	A_UTF16Char relinkedPath[AEGP_MAX_PATH_SIZE];

	fs::path path;
	fs::path pathRelinked;
};

class AeObjectNode : public ObjectCount
{
public:
	AeObjectNode(AEGP_PluginID pluginId, SPBasicSuite *sp, AEGP_ItemH theItemH, A_long theItemNr);
	AeObjectNode(AeObjectNode* node);
	bool isItemFooSolid() const;
	AEGP_ItemType getItemType() const;
	AEGP_PluginID getPluginId() const;
    std::string getItemIdString() const;
	A_long getItemId() const;
	A_long getItemNr() const;
	A_long getItemActCount() const;
	AEGP_ItemH getItemH() const;
	SPBasicSuite *getSp() const;
	AEGP_ItemFlags getItemFlags() const;
	void printInfo(FILE *dbgFile);
	void deleteItem();

	bool operator==(const AeObjectNode &obj2) const;
	bool operator==(const std::list<AeObjectNode*>::const_iterator &it) const;

	bool operator>=(const AeObjectNode &right) const;
	bool operator<(const AeObjectNode &right) const;

	static bool sortByUidMethodGrow(const AeObjectNode *left, const AeObjectNode *right);	//rosn¹co
	static bool sortByUidMethodShrink(const AeObjectNode *left, const AeObjectNode *right);	//malej¹co

protected:
	AEGP_PluginID pId;
	SPBasicSuite *sp;
	AEGP_ItemH itemH;
	AEGP_ItemType itemType;
	AEGP_ItemFlags itemFlags;	
	A_long uniqueID;
	A_long itemNr;
	A_long itemActCount;
};

typedef std::list<AeObjectNode*> aeObjNodes;
typedef aeObjNodes::const_iterator aeObjNodesIt;

#endif 
