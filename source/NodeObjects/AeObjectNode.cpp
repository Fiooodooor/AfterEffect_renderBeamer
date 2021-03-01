#include "AeObjectNode.h"


ObjectCount::ObjectCount() {
	count++;
}
int ObjectCount::showCount() {
	return count;
}
int ObjectCount::count = 0;

AeObjectNode::AeObjectNode(AEGP_PluginID pluginId, SPBasicSuite *sp_bs, AEGP_ItemH theItemH, A_long theItemNr)
	: itemType(0)
	, itemFlags(0)
	, uniqueID(0)
{
	pId = pluginId;
	this->sp = sp_bs;
	AEGP_SuiteHandler suites(sp_bs);
	this->itemH = theItemH;
	this->itemNr = theItemNr;
	suites.ItemSuite8()->AEGP_GetItemID(itemH, &uniqueID);
	suites.ItemSuite8()->AEGP_GetItemType(itemH, &itemType);
	suites.ItemSuite8()->AEGP_GetItemFlags(itemH, &itemFlags);
	itemActCount = showCount();
}
AeObjectNode::AeObjectNode(AeObjectNode* node)
{
	if (!node)
		return;
	this->pId = node->getPluginId();
	this->sp = node->getSp();
	this->itemH = node->getItemH();
	this->uniqueID = node->getItemId();
	this->itemNr = node->getItemNr();
	this->itemType = node->getItemType();
	this->itemFlags = node->getItemFlags();
	itemActCount = showCount();
	delete node;
	node = nullptr;
}

bool AeObjectNode::operator==(const AeObjectNode &right) const
{
	return getItemId() == right.getItemId();
}
bool AeObjectNode::operator==(const std::list<AeObjectNode*>::const_iterator &it) const
{
	return operator==(*(*it));
}
bool AeObjectNode::operator>=(const AeObjectNode &right) const
{
	if(getItemId() >= right.getItemId())
		return true;
	return false;
}
bool AeObjectNode::operator<(const AeObjectNode &right) const
{
	return !operator>=(right);
}
bool AeObjectNode::sortByUidMethodGrow(const AeObjectNode *left, const AeObjectNode *right)
{
	return *left < *right;
}
bool AeObjectNode::sortByUidMethodShrink(const AeObjectNode *left, const AeObjectNode *right)
{
	return !sortByUidMethodGrow(left, right);
}
bool AeObjectNode::isItemFooSolid() const
{
	AEGP_SuiteHandler suites(sp);
	AEGP_FootageH tmpFooH = nullptr;
	AEGP_FootageSignature tmpSignature = 0;
	A_Err err = A_Err_NONE;
	if (this->getItemType() == AEGP_ItemType_FOOTAGE) {
		ERR(suites.FootageSuite5()->AEGP_GetMainFootageFromItem(this->getItemH(), &tmpFooH));
		ERR(suites.FootageSuite5()->AEGP_GetFootageSignature(tmpFooH, &tmpSignature));
		return (err == A_Err_NONE) && (tmpSignature == AEGP_FootageSignature_SOLID);
	}
	return this->getItemType() == AEGP_ItemType_SOLID_defunct;
}

AEGP_ItemType AeObjectNode::getItemType() const { return this->itemType; }
AEGP_PluginID AeObjectNode::getPluginId() const { return this->pId;  };
std::string AeObjectNode::getItemIdString() const { return std::to_string(this->uniqueID); }
A_long AeObjectNode::getItemId() const            { return this->uniqueID; }
A_long AeObjectNode::getItemNr() const			{ return this->itemNr; }
A_long AeObjectNode::getItemActCount() const	{ return this->itemActCount; }
AEGP_ItemH AeObjectNode::getItemH() const		{ return this->itemH; };
SPBasicSuite *AeObjectNode::getSp() const		{ return this->sp; }
AEGP_ItemFlags AeObjectNode::getItemFlags() const { return this->itemFlags; }

void AeObjectNode::printInfo(FILE *dbgFile)
{
	fprintf(dbgFile, "ItemH: %zd UID: %d", (size_t)getItemH(), getItemId());
}

void AeObjectNode::deleteItem()
{
	AEGP_SuiteHandler suites(sp);
	if(itemH)
		suites.ItemSuite9()->AEGP_DeleteItem(itemH);
	itemH = nullptr;
}
