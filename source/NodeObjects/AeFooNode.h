#pragma once
#include "AeObjectNode.h"
#include "../GF_GlobalClasses.h"

class AeFootageNode : public AeObjectNode, public AePathNode
{
public:
	AeFootageNode(AeObjectNode* objNode);
	long generateFootData();
	AEGP_FootageH getFooH() const;
	bool isFooFile() const;
	bool isFooMissing() const;
	A_long getNrFiles() const;
	bool isFooSequence() const;

	A_long copyOf;
	AeFootageNode* copyFootageNode;
	
	A_long relinked, size;
	AEGP_ItemFlags fooFlags;
	AEGP_FootageLayerKey layerKey;
	AEGP_FootageSignature signature;
	AEGP_FootageInterp interpret;
	AEGP_FileSequenceImportOptions seqImpOptions;
	AEGP_FootageH fooH, newFooH;

protected:
	A_long nrFiles;
	A_long nrFpFrame;
};

typedef std::list<AeFootageNode*> aeFooNodes;
typedef aeFooNodes::const_iterator aeFooNodesIt;