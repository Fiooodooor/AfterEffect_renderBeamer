#include "GF_AEGP_Strings.h"

TableString    g_strs[StrID_NUMTYPES] = {
	{StrID_NONE,					""},
	{StrID_Name,					GF_PLUGIN_VERSION( GF_PLUGIN_VERSION_MAJOR )},
	{StrID_MenuEdit,				"renderBeamer Collect All"},
	{StrID_MenuEditSmart,			"renderBeamer Collect Smart"},
	{StrID_MenuBatch,				"renderBeamer Batch Relinker"},
	{StrID_MenuUiCollect,			"renderBeamer_ui_function"},
	{StrID_MenuUiLocation,			"(Windows->Extensions->renderBeamer)"},
	{StrID_MenuExport,				"renderBeamer"},
	{StrID_MenuVersion,			"renderBeamer Version"},
	{StrID_MenuCost,				"renderBeamer CostCalc"},
	{StrID_AboutMenu,				"renderBeamer about"},
	{StrID_Description,			"renderBeamer, Copyright 2020 GarageFarm.NET. Render farm uploader plug-in."},
	{StrID_LoadPluginError,		"There was an error in renderBeamer plugin load function. Contact support."},
	{StrID_AboutDialogText,		GF_PLUGIN_VERSION( GF_PLUGIN_VERSION_MAJOR )	},
	{StrID_CollectSmartWarning,	"Warning!\n Use \"Collect Smart\" carefully. It does not include compositions or footages dependencies inside scripts or expressions."},
	{StrID_OutputFormatPNGWarning,	"Warning!\n Your project will be rendered to PNG frames and converted to video after complete rendering."},
	{StrID_ProjectSent,			"Your project has been succesfuly forwarded to renderBeamer."}
};
// renderBeamer for After Effects v.17

TableString beamerMaskTable[_BeamerMask_ItemsN] = {
	{BeamerMask_GetUser, "-getusr -f \"%ls\""},
	{BeamerMask_GetTemp, "-tempdir -f \"%ls\""},
	{BeamerMask_SendTask, "-app AfterEffects -a \"%ls\" -sn \"%ls\" -f \"%ls\""},
	{BeamerMask_CheckScene, "-checkName \"%ls\" %ls -app AfterEffects -f \"%ls\""},
	{BeamerMask_GetFont, "app.project.item(%ld).layer(%ld).property(\"Source Text\").value.font"},
	{BeamerMask_GetFontFamily, "app.project.item(%ld).layer(%ld).property(\"Source Text\").value.fontFamily"},
	{BeamerMask_GetFontPath, "app.project.item(%ld).layer(%ld).property(\"Source Text\").value.fontLocation"},
	{BeamerMask_SendTaskEncoded, "-app AfterEffects -a \"%hs\" -encoded -sn \"%ls\" -f \"%ls\""},
	{BeamerMask_SendLogFile, "-log \"%ls\" -logSubject %ls AfterEffects -f \"%ls\""}
};

A_char *GetStringPtr(int strNum) { return g_strs[strNum].str; }
A_char *GetBeamerMaskA(int strNum) { return beamerMaskTable[strNum].str; }

const wchar_t *GetBeamerMaskW(int strNum)
{
    static wchar_t data[512] = { '\0' };
#ifdef AE_OS_WIN
    RB_SWPRINTF(data, 512, L"%hs", GetBeamerMaskA(strNum));
#else
    swprintf(data, 512, L"%s", GetBeamerMaskA(strNum));
#endif
    return data;
}
