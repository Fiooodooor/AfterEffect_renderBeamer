#pragma once
#ifndef AECOMPNODE_HPP
#define AECOMPNODE_HPP
#include "AeObjectNode.h"
#include "../GF_GlobalClasses.h"
namespace RenderBeamer {
class AeFontNode : public AePathNode
{
public:
	AeFontNode(A_long theItemNr, A_long theLayerNr);
	long fillFont(SPBasicSuite *sp, AEGP_PluginID plugId);
	const A_char* getFont() const;
	const A_char* getFamily() const;
	const A_char* getLocation() const;
	bool operator==(const AeFontNode &right) const;
	bool operator==(const std::list<AeFontNode*>::const_iterator &right) const;

private:
	A_char font[AEGP_MAX_PATH_SIZE];
	A_char family[AEGP_MAX_PATH_SIZE];
	A_char location[AEGP_MAX_PATH_SIZE];
	A_long itemNr, layerNr;
};

class AeEffectNode
{
public:
	AeEffectNode(AEGP_InstalledEffectKey theInstalledKey);
	AEGP_InstalledEffectKey getKey() const;
	long loadEffectInfo(SPBasicSuite *sp);

	bool operator==(const AeEffectNode &right) const;
	bool operator==(const std::list<AeEffectNode*>::const_iterator &right) const;

	const A_char* getEffectName() const;
	const A_char* getEffectNameSafe() const;
	const A_char* getEffectMatchN() const;
	const A_char* getEffectCategory() const;
private:
	AEGP_InstalledEffectKey installedKey;
	A_char effectName[AEGP_MAX_EFFECT_NAME_SIZE];
	A_char effectNameSafe[AEGP_MAX_EFFECT_NAME_SIZE];
	A_char effectMatchN[AEGP_MAX_EFFECT_MATCH_NAME_SIZE];
	A_char effectCategory[AEGP_MAX_EFFECT_CATEGORY_NAME_SIZE];
};

class AeLayerNode
{
public:
	AeLayerNode(SPBasicSuite *sp, AEGP_LayerH theLayerH, A_long theLayerNr);
	AEGP_LayerH getLayerH() const;
	AEGP_ObjectType getLayerObjectType() const;
	AEGP_ItemH getLayerSource() const;
	A_long getEffectsN() const;
	bool doesLayerHaveSource() const;
	void setLayerNameExplicit(AEGP_PluginID plugId);

protected:
	SPBasicSuite *sp;
	AEGP_ItemH itemH;
	AEGP_LayerH	layerH;
	A_long layerNr;
	AEGP_ObjectType layerObjectType;
	A_long effectsN;
};

class AeCompNode : public AeObjectNode
{
public:
	AeCompNode(AeObjectNode* objNode);
	AEGP_CompH getCompHandle() const;
	A_long getLayersNumber() const;
	long generateLayers();

	std::list<AeLayerNode*> &getLayersList();
	std::list<AeEffectNode*> &getEffectsList();
	std::list<AeFontNode*> &getFontsList();

protected:
	long pushUniqueEffect(AEGP_InstalledEffectKey effect_key);
	long pushUniqueFont(AeFontNode* font_node);
	AEGP_CompH compH;
	A_long layersN;
	std::list<AeLayerNode*> layersList;
	std::list<AeEffectNode*> effectsList;
	std::list<AeFontNode*> fontsList;
};

typedef std::list<AeCompNode*> aeCmpNodes;
typedef aeCmpNodes::const_iterator aeCmpNodesIt;

} // namespace RenderBeamer
#endif
