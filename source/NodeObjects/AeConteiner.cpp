#include "AeConteiner.h"

AeSceneContainer::AeSceneContainer()
{
	nrOfCollectedItems = 0;
}
void AeSceneContainer::countCollectedNr() 
{
	nrOfCollectedItems = static_cast<A_long>(renderCompositionSortedList.size());
	nrOfCollectedItems += static_cast<A_long>(renderFootageSortedList.size());
	nrOfCollectedItems += static_cast<A_long>(effectsList.size());
	nrOfCollectedItems += static_cast<A_long>(fontsList.size());
}
A_long AeSceneContainer::getCollectedNr() const
{
	return nrOfCollectedItems;
}