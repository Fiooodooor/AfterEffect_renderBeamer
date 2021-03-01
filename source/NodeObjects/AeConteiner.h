#pragma once
#ifndef AESCENECONTEINER_H
#define AESCENECONTEINER_H

#include "../GF_GlobalTypes.h"
#include "AeObjectNode.h"
#include "AeCompNode.h"
#include "AeFooNode.h"
#include "../AeGfsFileCreatorStructures.h"

class AeSceneConteiner
{
public:
	AeSceneConteiner();
	void countCollectedNr();
	A_long getCollectedNr() const;
	
	aeCmpNodes renderCompositionSortedList;
	aeFooNodes renderFootageSortedList;
	aeFooNodes renderFootageMissingList;
	aeObjNodes renderOtherSortedList;

	std::list<AeEffectNode*> effectsList;
	std::list<AeFontNode*> fontsList;
	std::list<gfsRqItem*> gfsRqItemsList;

private:
	A_long nrOfCollectedItems;
};

#endif