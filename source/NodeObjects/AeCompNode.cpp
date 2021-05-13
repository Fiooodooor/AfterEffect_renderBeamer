#include "AeCompNode.h"
namespace RenderBeamer {
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
	
	rbUtilities::gfsGetLeaveAllowedOnly(effectName, effectName_safe);
	rbUtilities::gfsGetLeaveAllowedOnly(effectMatchN, effectMatchN_safe);
	rbUtilities::gfsGetLeaveAllowedOnly(effectCategory, effectCategory_safe);
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
	return &effectName_safe[0];
}
const A_char* AeEffectNode::getEffectMatchNSafe() const
{
	return &effectMatchN_safe[0];
}
const A_char* AeEffectNode::getEffectCategorySafe() const
{
	return &effectCategory_safe[0];
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
	: layerObjectType(AEGP_ObjectType_NONE), effectsN(0)
{
	this->sp = sp;
	AEGP_SuiteHandler suites(sp);
	layerNr = theLayerNr;
	layerH = theLayerH;
	
	suites.LayerSuite8()->AEGP_GetLayerObjectType(theLayerH, &layerObjectType);
	if (layerObjectType == AEGP_ObjectType_AV) {
		suites.LayerSuite8()->AEGP_GetLayerSourceItem(theLayerH, &itemH);
	}
	else {
		itemH = nullptr;
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
void AeLayerNode::setLayerNameExplicit(AEGP_PluginID plugId)
{
	AEGP_MemHandle layerName, layerSourceName;
	std::wstring layerNameString, layerSourceNameString;
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(sp);
	ERR(suites.LayerSuite8()->AEGP_GetLayerName(plugId, layerH, &layerName, &layerSourceName));
	ERR(rbUtilities::copyMemhUTF16ToString(sp, layerName, layerNameString));
	ERR(rbUtilities::copyMemhUTF16ToString(sp, layerSourceName, layerSourceNameString));

	if(layerNameString.empty() && !layerSourceNameString.empty())
	{
		A_UTF16Char new_name[AEGP_MAX_ITEM_NAME_SIZE] = { '\0' };
		rbUtilities::toUTF16(layerSourceNameString.c_str(), new_name, AEGP_MAX_ITEM_NAME_SIZE-1);
		ERR(suites.LayerSuite8()->AEGP_SetLayerName(layerH, new_name));
	}	
}

AeCompNode::AeCompNode(AeObjectNode* objNode) : AeObjectNode(objNode), compH(nullptr), layersN(0)
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
	AEGP_EffectRefH effectH = nullptr;
	AEGP_InstalledEffectKey effectKey = NULL;
	AEGP_LayerH tmpLayerH = nullptr;
	AeLayerNode* tmpLayerNode = nullptr;
	for (A_long i = 0; i < getLayersNumber(); ++i) {
		suites.LayerSuite8()->AEGP_GetCompLayerByIndex(compH, i, &tmpLayerH);
		tmpLayerNode = new AeLayerNode(sp, tmpLayerH, i);
		if (tmpLayerNode) {
			tmpLayerNode->setLayerNameExplicit(getPluginId());
			layersList.push_back(tmpLayerNode);
			if (tmpLayerNode->getLayerObjectType() == AEGP_ObjectType_TEXT) {
				auto* font_node = new AeFontNode(this->getItemNr(), i + 1);
				if (font_node) {
					font_node->fillFont(getSp(), getPluginId());
					pushUniqueFont(font_node);
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

long AeCompNode::pushUniqueEffect(AEGP_InstalledEffectKey effect_key)
{
	auto it = effectsList.cbegin();
	const auto end = effectsList.cend();
	
	while (it != end) {
		if (effect_key == (*it)->getKey())
			break;
		++it;
	}
	if (it == end) {
		auto *effect_node = new AeEffectNode(effect_key);
		if (!effect_node)
			return 1;
		effectsList.push_back(effect_node);
		return 0;
	}	
	return 2;
}

long AeCompNode::pushUniqueFont(AeFontNode* font_node)
{
	if (!font_node)
		return 1;

	auto it = fontsList.cbegin();
	const auto end = fontsList.cend();
	while (it != end) {
		if ((*font_node) == it)
			break;
		++it;
	}
	if (it == end) {
		fontsList.push_back(font_node);
		return 0;
	}	

	delete font_node;
	font_node = nullptr;
	return 2;
}
} // namespace RenderBeamer
