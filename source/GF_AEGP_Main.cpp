
#include "GF_AEGP_Main.h"
#include "NodeObjects/AeSceneCollector.h"

namespace RenderBeamer {

Renderbeamer::Renderbeamer(SPBasicSuite *pica_basicP, AEGP_PluginID pluginID) : p_basic_(pica_basicP) , pluginId(pluginID), suites(pica_basicP)
{
}
void Renderbeamer::ValuesConstructor()
{
	ERROR_CATCH_START
		if (values_constructor_called == true)
			return;
		values_constructor_called = true;

		PT_ETX(suites.CommandSuite1()->AEGP_GetUniqueCommand(&beamerEditCmd))
		PT_ETX(suites.CommandSuite1()->AEGP_InsertMenuCommand(beamerEditCmd, GetStringPtr(StrID_MenuBatch), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))

		PT_ETX(suites.CommandSuite1()->AEGP_GetUniqueCommand(&beamerCostCalcCmd))
		PT_ETX(suites.CommandSuite1()->AEGP_InsertMenuCommand(beamerCostCalcCmd, GetStringPtr(StrID_MenuCost), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))
	
		PT_ETX(suites.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, AEGP_Command_ALL, &Renderbeamer::SCommandHook, nullptr))
		PT_ETX(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(pluginId, &Renderbeamer::SUpdateMenuHook, nullptr))
	ERROR_CATCH_END_NO_INFO
}
void Renderbeamer::CommandHook(AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB)
{
	ERROR_CATCH_START//_MOD(MainCommandHookModule)
		*handledPB = TRUE;		
		if (command == beamerEditCmd) {
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
		ERROR_AEER(suites.CommandSuite1()->AEGP_EnableCommand(beamerCostCalcCmd))
	ERROR_CATCH_END(suites)
}
void Renderbeamer::DumpProject(A_Boolean useUiExporter)
{
	ERROR_CATCH_START
		GF_Dumper project_dumper(p_basic_, pluginId);
        beamerParamsStruct paths_structure = {};
		AeSceneContainer scene_items_container;
		const auto undo = suites.UtilitySuite6()->AEGP_StartUndoGroup("Renderbeamer");

		auto *dlg_ptr = project_dumper.get_progress_dialog(true, true, 0);
		MAIN_PROGRESS_THROW(*dlg_ptr, 2, 10)
			
		ERROR_RETURN_VOID(GF_Dumper::PreCheckProject(p_basic_, pluginId, paths_structure))
		ERROR_THROW(project_dumper.setPathsStruct(paths_structure))

		MAIN_PROGRESS_THROW(*dlg_ptr, 3, 10)
		ERROR_RETURN_VOID(project_dumper.PrepareProject())

		AeSceneCollector collector(pluginId, p_basic_, project_dumper.rootProjH, scene_items_container);
		ERROR_THROW(collector.collectSceneRenderQueueItems())
		MAIN_PROGRESS_THROW(*dlg_ptr, 9, 10)
		ERROR_THROW(project_dumper.setConteiner(scene_items_container))

		if (project_dumper.SetupUiQueueItems() == UserDialogCancel)
			throw PluginError(GF_PLUGIN_LANGUAGE, UserDialogCancel);
		ERROR_THROW(collector.AeSceneCollect(useUiExporter))

		MAIN_PROGRESS_THROW(*dlg_ptr, 12, 20)
		ERROR_RETURN_VOID(project_dumper.newBatchDumpProject())
		if(undo == A_Err_NONE) suites.UtilitySuite6()->AEGP_EndUndoGroup();
		ERROR_AEER(suites.UtilitySuite6()->AEGP_ReportInfo(pluginId, GetStringPtr(StrID_ProjectSent)))
		
	ERROR_CATCH_END(suites)
}

void Renderbeamer::CostCalculator() const
{
	ERROR_CATCH_START
		ERROR_THROW(rbUtilities::openCostCalculator(p_basic_))
	ERROR_CATCH_END(suites)
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
		reinterpret_cast<Renderbeamer*>(plugin_refconP)->CommandHook(command, hook_priority, already_handledB, handledPB);
	} PT_XTE_CATCH_RETURN_ERR
}
A_Err Renderbeamer::SNewRenderbeamer(AEGP_GlobalRefcon *global_refconP, SPBasicSuite *pica_basicP, AEGP_PluginID pluginID)
{
	ERROR_CATCH_START
		if (!pica_basicP)
			return A_Err_ALLOC;
	auto *plugin_pt = new Renderbeamer(pica_basicP, pluginID);
	if (!plugin_pt)
		return A_Err_ALLOC;

	plugin_pt->ValuesConstructor();
	*global_refconP = reinterpret_cast<AEGP_GlobalRefcon>(plugin_pt);

	if (!global_refconP)
		return A_Err_ALLOC;

	ERROR_CATCH_END_NO_INFO
	if (_ErrorCode == NoError)
		return A_Err_NONE;
	return A_Err_ALLOC;
}
} // namespace RenderBeamer

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
	
	ERR(RenderBeamer::Renderbeamer::SNewRenderbeamer(global_refconP, pb, pid));
	return err;
}
