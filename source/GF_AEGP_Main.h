#pragma once
#ifndef GF_AEGP_MAIN_H
#define GF_AEGP_MAIN_H

#include "GF_GlobalTypes.h"
#include "GF_AEGP_Strings.h"
#include "GF_AEGP_Dumper.h"
 
class Renderbeamer
{
	public:
	    SPBasicSuite                    *p_basic_ = nullptr;
	    AEGP_PluginID                    pluginId = 0;
		AEGP_SuiteHandler                suites;
		SuiteHelper<AEGP_PanelSuite1>	 panels_;
	    AEGP_Command                     beamerEditCmd =  0, beamerCostCalcCmd = 0, beamerUiBatchExport = 0;
	    const A_u_char*                  i_match_nameZ;

    Renderbeamer(SPBasicSuite *pica_basicP, AEGP_PluginID pluginID);
	void ValuesConstructor();

	static A_Err SNewRenderbeamer(AEGP_GlobalRefcon *global_refconP, SPBasicSuite *pica_basicP, AEGP_PluginID pluginID);
    static A_Err SCreatePanelHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CreatePanelRefcon refconP, AEGP_PlatformViewRef container, AEGP_PanelH panelH, AEGP_PanelFunctions1* outFunctionTable, AEGP_PanelRefcon* outRefcon);
	static A_Err SUpdateMenuHook(AEGP_GlobalRefcon plugin_refconP, AEGP_UpdateMenuRefcon refconP, AEGP_WindowType active_window);
	static SPAPI A_Err SCommandHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CommandRefcon refcon_pt, AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB);

private:	
    void CreatePanelHook(AEGP_PlatformViewRef container, AEGP_PanelH panelH, AEGP_PanelFunctions1* outFunctionTable, AEGP_PanelRefcon* outRefcon) { }
    void UpdateMenuHook(AEGP_WindowType active_window) const;
    void CommandHook(AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB);

	void DumpProject(A_Boolean useUiExporter = FALSE);
	void PluginVersion();
	void CostCalculator();
	bool values_constructor_called = false;
};

extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunction;

#endif
