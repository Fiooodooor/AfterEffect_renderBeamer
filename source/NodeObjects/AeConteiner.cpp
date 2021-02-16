#include "AeConteiner.h"

AeSceneConteiner::AeSceneConteiner()
{
	nrOfCollectedItems = 0;
}
void AeSceneConteiner::countCollectedNr() 
{
	nrOfCollectedItems = static_cast<A_long>(renderCompositionSortedList.size());
	nrOfCollectedItems += static_cast<A_long>(renderFootageSortedList.size());
	nrOfCollectedItems += static_cast<A_long>(effectsList.size());
	nrOfCollectedItems += static_cast<A_long>(fontsList.size());
}
A_long AeSceneConteiner::getCollectedNr() const
{
	return nrOfCollectedItems;
}