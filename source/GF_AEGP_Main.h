#ifndef GF_AEGP_MAIN_H
#define GF_AEGP_MAIN_H

#include "GF_GlobalTypes.h"
#include "GF_AEGP_Strings.h"
#include "GF_AEGP_Dumper.h"

namespace RenderBeamer {
typedef struct _AEGP_GlobalRefcon				*AEGP_GlobalRefcon1;

class Renderbeamer
{
public:
	SPBasicSuite *p_basic_ = nullptr;
	AEGP_PluginID pluginId = 0;
	AEGP_SuiteHandler suites;
	AEGP_Command beamerEditCmd = 0, beamerCostCalcCmd = 0;
	
    Renderbeamer(SPBasicSuite *pica_basicP, AEGP_PluginID pluginID);
	void ValuesConstructor();

	static A_Err SNewRenderbeamer(AEGP_GlobalRefcon *global_refconP, SPBasicSuite *pica_basicP, AEGP_PluginID pluginID);
	static A_Err SUpdateMenuHook(AEGP_GlobalRefcon plugin_refconP, AEGP_UpdateMenuRefcon refconP, AEGP_WindowType active_window);
	static SPAPI A_Err SCommandHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CommandRefcon refcon_pt, AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB);

	std::mutex main_mutex;
private:	
    void UpdateMenuHook(AEGP_WindowType active_window) const;
    void CommandHook(AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB);
	void DumpProject(A_Boolean useUiExporter = FALSE);	
	void CostCalculator() const;
	bool values_constructor_called = false;
};
} // namespace RenderBeamer
extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunction;

#endif
