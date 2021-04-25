
#include "GF_AEGP_Main.h"
#include "NodeObjects/AeSceneCollector.h"
#ifdef AE_OS_WIN
	#include "Socket/socket_win.h"
#else
	#include "socket/socket_macos.h"
#endif
#include "Socket/gfs_rq_node_wrapper.h"
template <> const A_char* SuiteTraits<AEGP_PanelSuite1>::i_name = kAEGPPanelSuite;
template <> const int32_t SuiteTraits<AEGP_PanelSuite1>::i_version = kAEGPPanelSuiteVersion1;

//A_char script[] = "var dlg = new Window('dialog', 'Precheck assets error!'); var list = dlg.add('ListBox', [0, 0, 300, 300], 'asd', { numberOfColumns: 2, showHeaders : true,columnTitles : ['File name', 'File path'] }); dlg.show();";
//A_Err errRes = suites.UtilitySuite6()->AEGP_ExecuteScript(i_pluginID, script, FALSE, NULL, NULL);
//
// dodac urzycie basic_string<char> zamiast string.
/*
//	char txt[] = "To jest moj tekst";
	//std::basic_string<char16_t> mytxt = { txt, txt + 17 };
//	suites.UtilitySuite6()->AEGP_ReportInfoUnicode(pluginId, (A_UTF16Char*)mytxt.c_str());
*/
A_Err Renderbeamer::SNewRenderbeamer(AEGP_GlobalRefcon *global_refconP, SPBasicSuite *pica_basicP, AEGP_PluginID pluginID)
{
	if (!pica_basicP)
		return A_Err_ALLOC;
	auto *plugin_pt = new Renderbeamer(pica_basicP, pluginID);
	if (!plugin_pt)
		return A_Err_ALLOC;

	plugin_pt->ValuesConstructor();
	*global_refconP = (AEGP_GlobalRefcon)plugin_pt;

	if (!global_refconP)
		A_Err_ALLOC;

	return A_Err_NONE;
}
Renderbeamer::Renderbeamer(SPBasicSuite *pica_basicP, AEGP_PluginID pluginID) : p_basic_(pica_basicP) , pluginId(pluginID)
	, suites(pica_basicP), panels_(pica_basicP), i_match_nameZ( (A_u_char*) GetStringPtr(StrID_Name))
{
}
void Renderbeamer::ValuesConstructor()
{
	ERROR_CATCH_START
		if (values_constructor_called == true)
			return;
		values_constructor_called = false;

		PT_ETX(suites.CommandSuite1()->AEGP_GetUniqueCommand(&beamerEditCmd))
		PT_ETX(suites.CommandSuite1()->AEGP_InsertMenuCommand(beamerEditCmd, GetStringPtr(StrID_MenuBatch), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))
		PT_ETX(suites.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerEditCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
		PT_ETX(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(pluginId, &Renderbeamer::SUpdateMenuHook, NULL))

		PT_ETX(suites.CommandSuite1()->AEGP_GetUniqueCommand(&beamerUiBatchExport))
		PT_ETX(suites.CommandSuite1()->AEGP_InsertMenuCommand(beamerUiBatchExport, GetStringPtr(StrID_MenuUiCollect), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))
		PT_ETX(suites.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerUiBatchExport, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))

		PT_ETX(suites.CommandSuite1()->AEGP_GetUniqueCommand(&beamerCostCalcCmd))
		PT_ETX(suites.CommandSuite1()->AEGP_InsertMenuCommand(beamerCostCalcCmd, GetStringPtr(StrID_MenuCost), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))
		PT_ETX(suites.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerCostCalcCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
	ERROR_CATCH_END_NO_INFO
}
void Renderbeamer::CommandHook(
    AEGP_Command        command,                /* >> */
    AEGP_HookPriority    hook_priority,            /* >> currently always AEGP_HP_BeforeAE */
    A_Boolean            already_handledB,        /* >> */
    A_Boolean            *handledPB)                /* << whether you handled */
{
	ERROR_CATCH_START//_MOD(MainCommandHookModule)
		*handledPB = TRUE;		
		if (command == beamerUiBatchExport || command == beamerEditCmd) {
			DumpProject(TRUE);
		}	
		else if(command == beamerCostCalcCmd) {
			CostCalculator();
        }
		else {
			*handledPB = FALSE;
		}
	ERROR_CATCH_END(suites)
}

void Renderbeamer::UpdateMenuHook(AEGP_WindowType active_window) const
{
	ERROR_CATCH_START
		ERROR_AEER(suites.CommandSuite1()->AEGP_EnableCommand(beamerEditCmd))
		ERROR_AEER(suites.CommandSuite1()->AEGP_EnableCommand(beamerUiBatchExport))		
		ERROR_AEER(suites.CommandSuite1()->AEGP_EnableCommand(beamerCostCalcCmd))
	ERROR_CATCH_END(suites)
}
void Renderbeamer::DumpProject(A_Boolean useUiExporter)
{
	ERROR_CATCH_START
		GF_Dumper project_dumper(p_basic_, pluginId);
		PF_AppProgressDialogP *dlg_ptr = project_dumper.get_progress_dialog(true, true, 0);
		MAIN_PROGRESS_THROW(*dlg_ptr, 1, 10)
		
		platform_socket connector;	
		beamerParamsStruct paths_structure;
		AeSceneContainer scene_items_container;
	
		MAIN_PROGRESS_THROW(*dlg_ptr, 2, 10)
		ERROR_THROW(GF_Dumper::PreCheckProject(p_basic_, pluginId, paths_structure))
		ERROR_THROW(project_dumper.setPathsStruct(paths_structure))
		MAIN_PROGRESS_THROW(*dlg_ptr, 3, 10)
		ERROR_THROW(project_dumper.PrepareProject())
	
		AeSceneCollector collector(pluginId, p_basic_, project_dumper.rootProjH, scene_items_container);
		ERROR_THROW(collector.AeSceneCollect(useUiExporter))
		MAIN_PROGRESS_THROW(*dlg_ptr, 9, 10)

		ERROR_LONG_ERR(connector.start_session(paths_structure.socketPort_long, paths_structure.bp.projectRootCorrect.string()))
	
		std::string send_buffer;
		char read_buffer[10000];
		gfs_rq_node_wrapper::serialize(scene_items_container, send_buffer);
		connector.write(send_buffer.c_str(), static_cast<unsigned long>(send_buffer.length()));
	
		MAIN_PROGRESS_THROW(*dlg_ptr, 10, 20)
		while(connector.is_connected() && _ErrorCode == NoError)
		{						
			if (connector.read(read_buffer, 10000 - 1) > 0) {
				GF_Dumper::rbProj()->logg("read the buffer", "success", read_buffer);
				break;
			}
			MAIN_PROGRESS_THROW(*dlg_ptr, 11, 20)
		}
		ERROR_LONG_ERR(connector.close_socket())
	
		ERROR_THROW(project_dumper.setConteiner(scene_items_container))
		MAIN_PROGRESS_THROW(*dlg_ptr, 12, 20)
		ERROR_THROW(project_dumper.newBatchDumpProject())
		ERROR_AEER(suites.UtilitySuite6()->AEGP_ReportInfo(pluginId, GetStringPtr(StrID_ProjectSent)))		
	ERROR_CATCH_END(suites)
}

void Renderbeamer::PluginVersion() 
{
	ERROR_CATCH_START
		// ERROR_AE(rbUtilities::getVersionString(tmp_message, 512))
		// ERROR_AEER(suites.UtilitySuite6()->AEGP_ReportInfo(pluginId, tmp_message))
	ERROR_CATCH_END(suites)
}

void Renderbeamer::CostCalculator() 
{
	ERROR_CATCH_START
		ERROR_THROW(rbUtilities::openCostCalculator(p_basic_))
	ERROR_CATCH_END(suites)
}

A_Err Renderbeamer::SCreatePanelHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CreatePanelRefcon refconP, AEGP_PlatformViewRef container, AEGP_PanelH panelH, AEGP_PanelFunctions1* outFunctionTable, AEGP_PanelRefcon* outRefcon)
{
	PT_XTE_START
		reinterpret_cast<Renderbeamer*>(plugin_refconP)->CreatePanelHook(container, panelH, outFunctionTable, outRefcon);
	PT_XTE_CATCH_RETURN_ERR
}
A_Err Renderbeamer::SUpdateMenuHook(AEGP_GlobalRefcon plugin_refconP, AEGP_UpdateMenuRefcon refconP, AEGP_WindowType active_window)
{
	PT_XTE_START
		reinterpret_cast<Renderbeamer*>(plugin_refconP)->UpdateMenuHook(active_window);
	PT_XTE_CATCH_RETURN_ERR
}
A_Err Renderbeamer::SCommandHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CommandRefcon refcon_pt, AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB)
{
	*handledPB = FALSE;
	PT_XTE_START{
		reinterpret_cast<Renderbeamer*>(refcon_pt)->CommandHook(command, hook_priority, already_handledB, handledPB);
	} PT_XTE_CATCH_RETURN_ERR
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
