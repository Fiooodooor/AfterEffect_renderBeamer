#pragma once
#ifndef GF_AEGP_MAIN_H
#define GF_AEGP_MAIN_H

#include "GF_GlobalTypes.h"
#include "GF_AEGP_Strings.h"
#include "GF_AEGP_Dumper.h"
 
class Renderbeamer
{
public:
    SPBasicSuite                    *i_pica_basicP;
    AEGP_PluginID                    pluginId;
    AEGP_SuiteHandler                i_sp;
	SuiteHelper<AEGP_PanelSuite1>	 i_ps;
    AEGP_Command                     beamerEditCmd, beamerCostCalcCmd, beamerUiBatchExport;
    const A_u_char*                  i_match_nameZ;
    beamerParamsStruct               myPaths;
    
    Renderbeamer(SPBasicSuite *pica_basicP,    AEGP_PluginID pluginID);

	static A_Err SNewRenderbeamer(AEGP_GlobalRefcon *global_refconP, SPBasicSuite *pica_basicP, AEGP_PluginID pluginID)
	{
		try {
			if (pica_basicP)
				*global_refconP = (AEGP_GlobalRefcon) new Renderbeamer(pica_basicP, pluginID);
			if (!global_refconP || !pica_basicP)
				throw PluginError(A_Err_ALLOC);
		}
		catch (...) {
			AEGP_SuiteHandler sh(pica_basicP);
			sh.UtilitySuite6()->AEGP_ReportInfo(pluginID, GetStringPtr(StrID_LoadPluginError));
			return A_Err_ALLOC;
		}
		return A_Err_NONE;
	}

    static A_Err SCreatePanelHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CreatePanelRefcon refconP, AEGP_PlatformViewRef container, AEGP_PanelH panelH, AEGP_PanelFunctions1* outFunctionTable, AEGP_PanelRefcon* outRefcon)
    {
		PT_XTE_START{
			reinterpret_cast<Renderbeamer*>(plugin_refconP)->CreatePanelHook(container, panelH, outFunctionTable, outRefcon);
		} PT_XTE_CATCH_RETURN_ERR
    }

	static A_Err SUpdateMenuHook(
        AEGP_GlobalRefcon        plugin_refconP,        /* >> */
        AEGP_UpdateMenuRefcon    refconP,				/* >> */
        AEGP_WindowType          active_window)         /* >> */
    {
		PT_XTE_START{
			reinterpret_cast<Renderbeamer*>(plugin_refconP)->UpdateMenuHook(active_window);
		} PT_XTE_CATCH_RETURN_ERR
    }

    static SPAPI A_Err SCommandHook(
        AEGP_GlobalRefcon    plugin_refconP,		/* >> */
        AEGP_CommandRefcon   refconP,				/* >> */
        AEGP_Command         command,				/* >> */
        AEGP_HookPriority    hook_priority,			/* >> currently always AEGP_HP_BeforeAE */
        A_Boolean            already_handledB,		/* >> */
        A_Boolean            *handledPB)			/* << whether you handled */
	{
		*handledPB = FALSE;
		PT_XTE_START{
			reinterpret_cast<Renderbeamer*>(refconP)->CommandHook(command, hook_priority, already_handledB, handledPB);
		} PT_XTE_CATCH_RETURN_ERR
	}

private:
	A_char tmp_message[512] = { '\0' };
	A_Boolean output_prompt;
    void CreatePanelHook(AEGP_PlatformViewRef container, AEGP_PanelH panelH, AEGP_PanelFunctions1* outFunctionTable, AEGP_PanelRefcon* outRefcon) { }
    void UpdateMenuHook(AEGP_WindowType active_window) const;
    void CommandHook(AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB);

	void DumpProject(A_Boolean useUiExporter = FALSE);
	void PluginVersion();
	void CostCalculator();

};

extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunction;

#endif
