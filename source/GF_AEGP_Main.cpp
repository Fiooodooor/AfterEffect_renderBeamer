
#include "GF_AEGP_Main.h"
#include "NodeObjects/AeSceneCollector.h"
#ifdef AE_OS_WIN
	#include "socket/socket_win.h"
#else
	#include "socket/socket_macos.h"
#endif
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
	, beamerEditCmd(0), beamerCostCalcCmd(0), beamerUiBatchExport(0)
	, i_match_nameZ((A_u_char*)GetStringPtr(StrID_Name))
	, output_prompt(FALSE)
{
	
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerEditCmd))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerCostCalcCmd))
    PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&beamerUiBatchExport))

    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerEditCmd, GetStringPtr(StrID_MenuBatch), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))
	PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerUiBatchExport, GetStringPtr(StrID_MenuUiCollect), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))
    PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(beamerCostCalcCmd, GetStringPtr(StrID_MenuCost), AEGP_Menu_COMPOSITION, AEGP_MENU_INSERT_AT_BOTTOM))    

    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerEditCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerCostCalcCmd, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))
    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(pluginId, AEGP_HP_BeforeAE, beamerUiBatchExport, &Renderbeamer::SCommandHook, reinterpret_cast<AEGP_CommandRefcon>(this)))

    PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(pluginId, &Renderbeamer::SUpdateMenuHook, NULL))
}

void Renderbeamer::CommandHook(
    AEGP_Command        command,                /* >> */
    AEGP_HookPriority    hook_priority,            /* >> currently always AEGP_HP_BeforeAE */
    A_Boolean            already_handledB,        /* >> */
    A_Boolean            *handledPB)                /* << whether you handled */
{
	ERROR_CATCH_START//_MOD(MainCommandHookModule)
		*handledPB = TRUE;		
		if (command == beamerUiBatchExport) {		
			DumpProject(TRUE);			
		}	
		else if(command == beamerCostCalcCmd) {
			CostCalculator();
        }
		else if (command == beamerEditCmd) {
			DumpProject(FALSE);
		}
		else {
			*handledPB = FALSE;		
		}
	ERROR_CATCH_END(i_sp)
}

void Renderbeamer::UpdateMenuHook(AEGP_WindowType active_window) const
{
	ERROR_CATCH_START
		ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerEditCmd))
		ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerUiBatchExport))		
		ERROR_AEER(i_sp.CommandSuite1()->AEGP_EnableCommand(beamerCostCalcCmd))
	ERROR_CATCH_END(i_sp)
}
void Renderbeamer::DumpProject(A_Boolean useUiExporter) const
{
	ERROR_CATCH_START
		beamerParamsStruct paths_structure;
		ERROR_AE(GF_Dumper::PreCheckProject(i_pica_basicP, pluginId, &paths_structure))
		if (_ErrorCode == NoError) 
		{			
			GF_Dumper project_dumper(i_pica_basicP, pluginId, &paths_structure);
			ERROR_AE(project_dumper.PrepareProject())
			if (_ErrorCode == NoError)
			{
				AeSceneConteiner scene_items_container;
				AeSceneCollector collector(pluginId, i_pica_basicP, project_dumper.rootProjH, scene_items_container);
				ERROR_AE(collector.AeSceneCollect(useUiExporter))
				if (_ErrorCode == NoError)
				{
					platform_socket connector;
					if (connector.create_socket() && connector.connect(static_cast<unsigned short>(paths_structure.socketPort_long)))
					{
						char buffer[10000] = { '\0' };
						std::string header = "SETUP=AE\tNAME=myscenename\n";// +paths_structure.bp.projectRootCorrect.string();

													  //initRenderbeamerPanel( '{\"ignore_missings\":\"1\",\"smart_collect\":\"0\",\"data\":[{\"name\":\"PRZELOT BANG\",\"frame_range\":\"0to1295s1\",\"fps\":\"24.0\",\"out_is_sequence\":\"1\",\"file_ext_format\":\"mov\",\"file_ext\":\"mov\",\"renderable\":\"1\",\"audio_available_in_comp\":\"0\",\"audio_out_enabled\":\"0\",\"audio_depth\":\"2\",\"audio_channels\":\"2\",\"audio_sample_rate\":\"48000\",\"video_encoder\":\"\",\"video_pixel_format\":\"\",\"video_profile\":\"\",\"video_bitrate\":\"5000\",\"composition_id\":\"260\",\"rq_id\":\"1\",\"rq_out_id\":\"1\",\"width\":\"1600\",\"height\":\"1200\"},{\"name\":\"TITLE REVEAL\",\"frame_range\":\"0to719s1\",\"fps\":\"24.0\",\"out_is_sequence\":\"1\",\"file_ext_format\":\"avi\",\"file_ext\":\"avi\",\"renderable\":\"1\",\"audio_available_in_comp\":\"0\",\"audio_out_enabled\":\"0\",\"audio_depth\":\"2\",\"audio_channels\":\"2\",\"audio_sample_rate\":\"48000\",\"video_encoder\":\"\",\"video_pixel_format\":\"\",\"video_profile\":\"\",\"video_bitrate\":\"5000\",\"composition_id\":\"329\",\"rq_id\":\"2\",\"rq_out_id\":\"1\",\"width\":\"1600\",\"height\":\"1200\"}]}');
						std::string main_data = "SCRIPT=initRenderbeamerPanel( '{\"ignore_missings\":\"1\",\"smart_collect\":\"0\",\"data\":[{\"name\":\"PRZELOT BANG\",\"frame_range\":\"0to1295s1\",\"fps\":\"24.0\",\"out_is_sequence\":\"1\",\"file_ext_format\":\"mov\",\"file_ext\":\"mov\",\"renderable\":\"1\",\"audio_available_in_comp\":\"0\",\"audio_out_enabled\":\"0\",\"audio_depth\":\"2\",\"audio_channels\":\"2\",\"audio_sample_rate\":\"48000\",\"video_encoder\":\"\",\"video_pixel_format\":\"\",\"video_profile\":\"\",\"video_bitrate\":\"5000\",\"composition_id\":\"260\",\"rq_id\":\"1\",\"rq_out_id\":\"1\",\"width\":\"1600\",\"height\":\"1200\"},{\"name\":\"TITLE REVEAL\",\"frame_range\":\"0to719s1\",\"fps\":\"24.0\",\"out_is_sequence\":\"1\",\"file_ext_format\":\"avi\",\"file_ext\":\"avi\",\"renderable\":\"1\",\"audio_available_in_comp\":\"0\",\"audio_out_enabled\":\"0\",\"audio_depth\":\"2\",\"audio_channels\":\"2\",\"audio_sample_rate\":\"48000\",\"video_encoder\":\"\",\"video_pixel_format\":\"\",\"video_profile\":\"\",\"video_bitrate\":\"5000\",\"composition_id\":\"329\",\"rq_id\":\"2\",\"rq_out_id\":\"1\",\"width\":\"1600\",\"height\":\"1200\"}]}');\n";
						const char sendbuf[] = "SCRIPT=initRenderbeamerPanel( '{\"ignore_missings\":\"1\",\"smart_collect\":\"0\",\"data\":[{\"name\":\"PRZELOT BANG\",\"frame_range\":\"0to1295s1\",\"fps\":\"24.0\",\"out_is_sequence\":\"1\",\"file_ext_format\":\"mov\",\"file_ext\":\"mov\",\"renderable\":\"1\",\"audio_available_in_comp\":\"0\",\"audio_out_enabled\":\"0\",\"audio_depth\":\"2\",\"audio_channels\":\"2\",\"audio_sample_rate\":\"48000\",\"video_encoder\":\"\",\"video_pixel_format\":\"\",\"video_profile\":\"\",\"video_bitrate\":\"5000\",\"composition_id\":\"260\",\"rq_id\":\"1\",\"rq_out_id\":\"1\",\"width\":\"1600\",\"height\":\"1200\"},{\"name\":\"TITLE REVEAL\",\"frame_range\":\"0to719s1\",\"fps\":\"24.0\",\"out_is_sequence\":\"1\",\"file_ext_format\":\"avi\",\"file_ext\":\"avi\",\"renderable\":\"1\",\"audio_available_in_comp\":\"0\",\"audio_out_enabled\":\"0\",\"audio_depth\":\"2\",\"audio_channels\":\"2\",\"audio_sample_rate\":\"48000\",\"video_encoder\":\"\",\"video_pixel_format\":\"\",\"video_profile\":\"\",\"video_bitrate\":\"5000\",\"composition_id\":\"329\",\"rq_id\":\"2\",\"rq_out_id\":\"1\",\"width\":\"1600\",\"height\":\"1200\"}]}');\n";
						connector.write(header.c_str(), static_cast<unsigned long>(header.length()));
						connector.write(sendbuf, static_cast<unsigned long>(strlen(sendbuf)));
						for(int a = 0; a < 4; a++)
						{
							GF_Dumper::rbProj()->logg("iteracja: ", std::to_string(a).c_str(), "");
							if (connector.read(buffer, 10000) > 0)
								GF_Dumper::rbProj()->logg(buffer, "", "");
						}
							// socket connect
							// socket send data
							// socket fetch return data
							// socket disconnect
						connector.close_socket();
					}
				}
				ERROR_AE(project_dumper.setConteiner(scene_items_container))
				ERROR_AE(project_dumper.newBatchDumpProject())
				ERROR_AEER(i_sp.UtilitySuite6()->AEGP_ReportInfo(pluginId, GetStringPtr(StrID_ProjectSent)))
			}
		}
		if (_ErrorCode != NoError)
		{
			i_sp.UtilitySuite6()->AEGP_ReportInfo(pluginId, PluginError::GetErrorStringA(_ErrorCode, UserLanguage::UserEnglish));
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
	AEGP_SuiteHandler suites(pb);
	AEGP_PersistentBlobH pbh;
	
	ERR(Renderbeamer::SNewRenderbeamer(global_refconP, pb, pid));
	
	if(suites.PersistentDataSuite4()->AEGP_GetApplicationBlob(AEGP_PersistentType_MACHINE_SPECIFIC, &pbh) == A_Err_NONE)
		suites.PersistentDataSuite4()->AEGP_DeleteEntry(pbh, "renderBeamer", "rq_items");
    return err;
}
