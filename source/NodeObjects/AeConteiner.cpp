#include "AeConteiner.h"

AeSceneContainer::AeSceneContainer()
{
	nrOfCollectedItems = 0;
}
AeSceneContainer::~AeSceneContainer()
{
	clear();
}
void AeSceneContainer::clear()
{
	ERROR_CATCH_START
		for (; !renderCompositionSortedList.empty(); renderCompositionSortedList.pop_back())
			delete renderCompositionSortedList.back();
	for (; !renderFootageSortedList.empty(); renderFootageSortedList.pop_back())
		delete renderFootageSortedList.back();
	for (; !renderFootageMissingList.empty(); renderFootageMissingList.pop_back())
		delete renderFootageMissingList.back();
	for (; !renderOtherSortedList.empty(); renderOtherSortedList.pop_back())
		delete renderOtherSortedList.back();
	for (; !effectsList.empty(); effectsList.pop_back())
		delete effectsList.back();
	for (; !fontsList.empty(); fontsList.pop_back())
		delete fontsList.back();
	for (; !gfsRqItemsList.empty(); gfsRqItemsList.pop_back())
		delete gfsRqItemsList.back();
	ERROR_CATCH_END_NO_INFO
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