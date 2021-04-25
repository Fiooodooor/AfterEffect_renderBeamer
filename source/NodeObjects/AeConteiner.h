#pragma once
#ifndef AESCENECONTEINER_H
#define AESCENECONTEINER_H

#include "../GF_GlobalTypes.h"
#include "AeObjectNode.h"
#include "AeCompNode.h"
#include "AeFooNode.h"
#include "../AeGfsFileCreatorStructures.h"

class AeSceneContainer
{
public:
	AeSceneContainer();
	void countCollectedNr();
	A_long getCollectedNr() const;
	
	aeCmpNodes renderCompositionSortedList;
	aeFooNodes renderFootageSortedList;
	aeFooNodes renderFootageMissingList;
	aeObjNodes renderOtherSortedList;

	std::list<AeEffectNode*> effectsList;
	std::list<AeFontNode*> fontsList;
	std::list<gfsRqItem*> gfsRqItemsList;
	A_Boolean ignore_missings_assets = 1;
	A_Boolean smart_collect = 0;

private:
	A_long nrOfCollectedItems;
};

#endif