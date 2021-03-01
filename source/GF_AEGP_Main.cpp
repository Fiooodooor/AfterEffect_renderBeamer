
#include "GF_AEGP_Main.h"
template <> const A_char* SuiteTraits<AEGP_PanelSuite1>::i_name = kAEGPPanelSuite;
template <> const int32_t SuiteTraits<AEGP_PanelSuite1>::i_version = kAEGPPanelSuiteVersion1;

//A_char script[] = "var dlg = new Window('dialog', 'Precheck assets error!'); var list = dlg.add('ListBox', [0, 0, 300, 300], 'asd', { numberOfColumns: 2, showHeaders : true,columnTitles : ['File name', 'File path'] }); dlg.show();";
//A_Err errRes = i_sp.UtilitySuite6()->AEGP_ExecuteScript(i_pluginID, script, FALSE, NULL, NULL);
//
// dodac urzycie basic_string<char> zamiast string.
/*
//	char txt[] = "To jest moj tekst";
	//std::basic_string<char16_t> mytxt = { txt, txt + 17 };
//	i_sp.UtilitySuite6()->AEGP_ReportInfoUnicode(pluginId, (A_UTF16Char*)mytxt.c_str());
*/

Renderbeamer::Renderbeamer(SPBasicSuite *pica_basicP, AEGP_PluginID pluginID)
    : i_pica_basicP(pica_basicP)
    , pluginId(pluginID)
    , i_sp(pica_basicP)
	, i_ps(pica_basicP)
	, beamerEditCmd(0), beamerExportCmd(0), beamerEditSmartCmd(0)
	, beamerBatchExport(0), beamerCostCalcCmd(0), beamerUiBatchExport(0)
	, i_match_nameZ((A_u_char*)GetStringPtr(StrID_Name))
	, smart_collect(FALSE)
	, output_prompt(FALSE)
{
	
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerEditCmd))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerEditSmartCmd))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerBatchExport))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerExportCmd))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerCostCalcCmd))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerUiBatchExport))

    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerEditCmd, GetStringPtr(StrID_MenuEdit), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_SORTED))
    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerEditSmartCmd, GetStringPtr(StrID_MenuEditSmart), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_SORTED))
    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerBatchExport, GetStringPtr(StrID_MenuBatch), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_SORTED))
    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerExportCmd, GetStringPtr(StrID_MenuExport), AEGP_Menu_EXPORT, AEGP_MENU_INSERT_SORTED))
    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerCostCalcCmd, GetStringPtr(StrID_MenuCost), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_SORTED))
    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerUiBatchExport, GetStringPtr(StrID_MenuUiCollect), AEGP_Menu_NONE, AEGP_MENU_INSERT_SORTED))

    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerEditCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerEditSmartCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerBatchExport, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerExportCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerCostCalcCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerUiBatchExport, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))

    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(pluginId, &Renderbeamer::SUpdateMenuHook, NULL))
	
	//i_sp.UtilitySuite6()->AEGP_WriteToDebugLog("subsystem", "event", "info");
}

void Renderbeamer::CommandHook(
    AEGP_Command        command,                /* >> */
    AEGP_HookPriority    hook_priority,            /* >> currently always AEGP_HP_BeforeAE */
    A_Boolean            already_handledB,        /* >> */
    A_Boolean            *handledPB)                /* << whether you handled */
{
	ERROR_CATCH_START//_MOD(MainCommandHookModule)
		*handledPB = TRUE;		
		if (command == beamerEditCmd || command == beamerExportCmd) {
			DumpProject(FALSE);
        }
		else if (command == beamerUiBatchExport) {
			DumpProject(FALSE, TRUE, TRUE);
		}
		else if(command == beamerBatchExport) {
			DumpProject(FALSE, TRUE);
		}
		else if (command == beamerEditSmartCmd) {
			if (smart_collect == FALSE) {
				ERROR_AEER(i_sp.UtilitySuite6()->AEGP_ReportInfo(pluginId, GetStringPtr(StrID_CollectSmartWarning)))
				smart_collect = TRUE;
			}
			DumpProject(TRUE);
		}
		else if(command == beamerCostCalcCmd) {
			CostCalculator();
        }
		else {
			*handledPB = FALSE;
		}
	ERROR_CATCH_END(i_sp)
}

void Renderbeamer::UpdateMenuHook(AEGP_WindowType active_window)
{
	ERROR_CATCH_START
		//ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerEditCmd))
		//ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerEditSmartCmd))
		ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerBatchExport))
		ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerUiBatchExport))
		//ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerExportCmd))
		ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerCostCalcCmd))
	ERROR_CATCH_END(i_sp)
}
void Renderbeamer::DumpProject(A_Boolean useSmartCollector, A_Boolean useBatchExporter, A_Boolean useUiExporter)
{
	ERROR_CATCH_START
		if(output_prompt == FALSE)
		{
			ERROR_AEER(i_sp.UtilitySuite6()->AEGP_ReportInfo(pluginId, GetStringPtr(StrID_OutputFormatPNGWarning)))
			output_prompt = TRUE;
		}
		ERROR_AE(GF_Dumper::PreCheckProject(i_pica_basicP, pluginId, &myPaths))
		if (_ErrorCode == NoError) 
		{			
			GF_Dumper tempDumper(i_pica_basicP, pluginId, &myPaths);
			ERROR_AE(tempDumper.PrepareProject())
			if (_ErrorCode == NoError) 
			{
				AeSceneConteiner SceneContainer;
				AeSceneCollector collector(pluginId, i_pica_basicP, tempDumper.rootProjH, SceneContainer);
				if (useSmartCollector == TRUE) {
					ERROR_AE(collector.AeSmartCollect())
				}
				else {
					ERROR_AE(collector.AeNormalCollect())
				}
				ERROR_AE(tempDumper.setConteiner(SceneContainer))
				if (useBatchExporter == FALSE) {
					ERROR_AE(tempDumper.newDumpProject())
				}
				else {
					ERROR_AE(tempDumper.newBatchDumpProject(useUiExporter))
				}
			}
		}
		// DEBUG_PLUGIN_DUMP_PROJ_END
	ERROR_CATCH_END_NO_INFO
}

void Renderbeamer::PluginVersion() 
{
	ERROR_CATCH_START
		ERROR_AE(rbUtilities::getVersionString(tmp_message, 512))
		ERROR_AEER(i_sp.UtilitySuite6()->AEGP_ReportInfo(pluginId, tmp_message))
	ERROR_CATCH_END(i_sp)
}

void Renderbeamer::CostCalculator() 
{
	ERROR_CATCH_START
		ERROR_THROW(rbUtilities::openCostCalculator(i_pica_basicP))
	ERROR_CATCH_END(i_sp)
}

A_Err EntryPointFunction(
    struct SPBasicSuite      *pica_basicP,            /* >> */
    A_long                   major_versionL,            /* >> */
    A_long                   minor_versionL,            /* >> */
    AEGP_PluginID            aegp_plugin_id,            /* >> */
    AEGP_GlobalRefcon        *global_refconP)        /* << */
{
	A_Err			err = A_Err_NONE;
	AEGP_PluginID	pid = aegp_plugin_id;
	SPBasicSuite	*pb = pica_basicP;
	
	ERR(Renderbeamer::SNewRenderbeamer(global_refconP, pb, pid));
    
    return err;
}
