#include "AeCompNode.h"

AeFontNode::AeFontNode(A_long theItemNr, A_long theLayerNr)
{
	font[0] = '\0';
	family[0] = '\0';
	location[0] = '\0';
	itemNr = theItemNr;
	layerNr = theLayerNr;
}
const A_char* AeFontNode::getFont() const
{
	return font;
}
const A_char* AeFontNode::getFamily() const
{
	return family;
}
const A_char* AeFontNode::getLocation() const
{
	return location;
}

bool AeFontNode::operator==(const AeFontNode &right) const
{
	if (strcmp(getFont(), right.getFont()) == 0) {
		if (strcmp(getLocation(), right.getLocation()) == 0) {
			return true;
		}
	}
	return false;
}
bool AeFontNode::operator==(const std::list<AeFontNode*>::const_iterator &right) const
{
	return operator==(*(*right));
}
long AeFontNode::fillFont(SPBasicSuite *sp, AEGP_PluginID plugId)
{
	return rbUtilities::getFontFromLayer(sp, plugId, itemNr-1, layerNr, font, family, location);
}


AeEffectNode::AeEffectNode(AEGP_InstalledEffectKey theInstalledKey)
	: installedKey(theInstalledKey)
{
	effectName[0] = '\0';
	effectNameSafe[0] = '\0';
	effectMatchN[0] = '\0';
	effectCategory[0] = '\0';
}
AEGP_InstalledEffectKey AeEffectNode::getKey() const
{
	return installedKey;
}
long AeEffectNode::loadEffectInfo(SPBasicSuite *sp)
{
	AEGP_SuiteHandler suites(sp);
	suites.EffectSuite4()->AEGP_GetEffectName(installedKey, effectName);
	suites.EffectSuite4()->AEGP_GetEffectMatchName(installedKey, effectMatchN);
	suites.EffectSuite4()->AEGP_GetEffectCategory(installedKey, effectCategory);
	suites.EffectSuite4()->AEGP_GetEffectName(installedKey, effectNameSafe);
	rbUtilities::leaveAllowedOnly(effectNameSafe);
	rbUtilities::leaveAllowedOnly(effectMatchN);
	rbUtilities::leaveAllowedOnly(effectCategory);
	return 0;
}

bool AeEffectNode::operator==(const AeEffectNode &right) const
{
	return getKey() == right.getKey();
}
bool AeEffectNode::operator==(const std::list<AeEffectNode*>::const_iterator &right) const
{
	return operator==(*(*right));
}
const A_char* AeEffectNode::getEffectNameSafe() const
{
	return effectNameSafe;
}
const A_char* AeEffectNode::getEffectName() const
{
	return effectName;
}
const A_char* AeEffectNode::getEffectMatchN() const
{
	return effectMatchN;
}
const A_char* AeEffectNode::getEffectCategory() const
{
	return effectCategory;
}

AeLayerNode::AeLayerNode(SPBasicSuite *sp, AEGP_LayerH theLayerH, A_long theLayerNr)
{
	this->sp = sp;
	AEGP_SuiteHandler suites(sp);
	layerNr = theLayerNr;
	
	suites.LayerSuite8()->AEGP_GetLayerObjectType(theLayerH, &layerObjectType);
	if (layerObjectType == AEGP_ObjectType_AV) {
		suites.LayerSuite8()->AEGP_GetLayerSourceItem(theLayerH, &itemH);
	}
	else {
		itemH = NULL;
	}
	suites.EffectSuite4()->AEGP_GetLayerNumEffects(theLayerH, &effectsN);
}
AEGP_LayerH AeLayerNode::getLayerH() const
{
	return layerH;
}
AEGP_ObjectType AeLayerNode::getLayerObjectType() const
{
	return layerObjectType;
}
AEGP_ItemH AeLayerNode::getLayerSource() const
{
	return itemH;
}
A_long AeLayerNode::getEffectsN() const
{
	return effectsN;
}
bool AeLayerNode::doesLayerHaveSource() const
{
	if (layerObjectType == AEGP_ObjectType_AV)
		return true;
	return false;
}

AeCompNode::AeCompNode(AeObjectNode* objNode) : AeObjectNode(objNode)
{
	AEGP_SuiteHandler suites(sp);
	suites.CompSuite10()->AEGP_GetCompFromItem(itemH, &compH);
	suites.LayerSuite8()->AEGP_GetCompNumLayers(compH, &layersN);
}
AEGP_CompH AeCompNode::getCompHandle() const
{
	return compH;
}
A_long AeCompNode::getLayersNumber() const
{
	return layersN;
}
std::list<AeLayerNode*> &AeCompNode::getLayersList()
{
	return layersList;
}
std::list<AeEffectNode*> &AeCompNode::getEffectsList()
{
	return effectsList;
}
std::list<AeFontNode*> &AeCompNode::getFontsList()
{
	return fontsList;
}

long AeCompNode::generateLayers()
{
	AEGP_SuiteHandler suites(sp);
	AEGP_EffectRefH effectH = NULL;
	AEGP_InstalledEffectKey effectKey = NULL;
	AEGP_LayerH tmpLayerH = NULL;
	AeLayerNode* tmpLayerNode = NULL;
	for (A_long i = 0; i < getLayersNumber(); ++i) {
		suites.LayerSuite8()->AEGP_GetCompLayerByIndex(compH, i, &tmpLayerH);
		tmpLayerNode = new AeLayerNode(sp, tmpLayerH, i);
		if (tmpLayerNode) {
			layersList.push_back(tmpLayerNode);
			if (tmpLayerNode->getLayerObjectType() == AEGP_ObjectType_TEXT) {
				AeFontNode* fontNode = new AeFontNode(this->getItemNr(), i + 1);
				if (fontNode) {
					fontNode->fillFont(getSp(), getPluginId());
					pushUniqueFont(fontNode);
				}
			}
		}
		for (A_long j = 0; j < tmpLayerNode->getEffectsN(); ++j) {
			suites.EffectSuite4()->AEGP_GetLayerEffectByIndex(pId, tmpLayerH, j, &effectH);
			suites.EffectSuite4()->AEGP_GetInstalledKeyFromLayerEffect(effectH, &effectKey);
			pushUniqueEffect(effectKey);
			suites.EffectSuite4()->AEGP_DisposeEffect(effectH);
		}
	}
	return 0;
}

long AeCompNode::pushUniqueEffect(AEGP_InstalledEffectKey theEffectKey)
{
	std::list<AeEffectNode*>::const_iterator it = effectsList.cbegin();
	std::list<AeEffectNode*>::const_iterator end = effectsList.cend();
	AeEffectNode *effectNode = new AeEffectNode(theEffectKey);
	if (!effectNode)
		return 1;

	while (it != end) {
		if ((*effectNode) == it)
			break;
		++it;
	}
	if (it == end) {
		effectsList.push_back(effectNode);
		return 0;
	}	
	delete effectNode;
	return 2;
}

long AeCompNode::pushUniqueFont(AeFontNode* fontNode)
{
	if (!fontNode)
		return 1;

	std::list<AeFontNode*>::const_iterator it = fontsList.cbegin();
	std::list<AeFontNode*>::const_iterator end = fontsList.cend();
	while (it != end) {
		if ((*fontNode) == it)
			break;
		++it;
	}
	if (it == end) {
		fontsList.push_back(fontNode);
		return 0;
	}	

	delete fontNode;
	fontNode = NULL;
	return 2;
}
