#include "GF_AEGP_MainUI.h"
#include <algorithm>

#undef min

template <> const A_char* SuiteTraits<PFAppSuite6>::i_name = kPFAppSuite;
template <> const int32_t SuiteTraits<PFAppSuite6>::i_version = kPFAppSuiteVersion6;

RenderbeamerUI::RenderbeamerUI(SPBasicSuite* spbP, 
								AEGP_PanelH panelH,
								AEGP_PlatformViewRef platformWindowRef,
								AEGP_PanelFunctions1* outFunctionTable)
	: i_refH(platformWindowRef)
	, i_panelH(panelH)
	, red(255), green(255), blue(255)
	, i_use_bg(false)
	, i_appSuite(spbP)
	, i_panelSuite(spbP)
	, i_numClicks(0)
	, appFontNum(0)
	, appFontSize(0)
	, appFontStyle(0)
	, progressStep(0)
	, progressTotal(10)
{
	outFunctionTable->DoFlyoutCommand = S_DoFlyoutCommand;
	outFunctionTable->GetSnapSizes = S_GetSnapSizes;
	outFunctionTable->PopulateFlyout = S_PopulateFlyout;
	
	i_appSuite->PF_GetFontStyleSheet(PF_FontStyle_APP, &appFontName, &appFontNum, &appFontSize, &appFontStyle);
}

void RenderbeamerUI::GetProgressBarRect(RECT drawRect, RECT &finishedR, RECT &unfinishedR)
{
	A_long delta;
	finishedR.top = drawRect.top + 2;
	unfinishedR.top = finishedR.top;

	finishedR.bottom = drawRect.bottom - 2;
	if (finishedR.bottom < finishedR.top + 16)
		finishedR.bottom = finishedR.top + 16;
	unfinishedR.bottom = finishedR.bottom;

	unfinishedR.left = drawRect.left + 10;
	finishedR.right = drawRect.right - 10;

	if (finishedR.right < unfinishedR.left + 20)
		finishedR.right = unfinishedR.left + 20;

	delta = finishedR.right - unfinishedR.left;
	unfinishedR.right = unfinishedR.left + ((delta * progressStep) / progressTotal);
	finishedR.left = unfinishedR.right;
}


void RenderbeamerUI::GetSnapSizes(A_LPoint*	snapSizes, A_long * numSizesP)
{
	snapSizes[0].x = 100;
	snapSizes[0].y = 100;
	snapSizes[1].x = 200;
	snapSizes[1].y = 400;
	*numSizesP = 2;
}

void RenderbeamerUI::PopulateFlyout(AEGP_FlyoutMenuItem* itemsP, A_long * in_out_numItemsP)
{
	AEGP_FlyoutMenuItem	myMenu[] = { 
		{1, AEGP_FlyoutMenuMarkType_NORMAL,		FALSE,	AEGP_FlyoutMenuCmdID_NONE,	reinterpret_cast<const A_u_char*>("Hi!")},
		{1, AEGP_FlyoutMenuMarkType_SEPARATOR,	TRUE,	AEGP_FlyoutMenuCmdID_NONE,	NULL  },
		{1, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	AEGP_FlyoutMenuCmdID_NONE,	reinterpret_cast<const A_u_char*>("Set BG Color")},
		{2, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	PT_MenuCmd_RED,				reinterpret_cast<const A_u_char*>("Red")},
		{2, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	PT_MenuCmd_GREEN,			reinterpret_cast<const A_u_char*>("Green")},
		{2, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	PT_MenuCmd_BLUE,			reinterpret_cast<const A_u_char*>("Blue")},
		{1, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	PT_MenuCmd_STANDARD,		reinterpret_cast<const A_u_char*>("Normal Fill Color")},
		{1, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	AEGP_FlyoutMenuCmdID_NONE,	reinterpret_cast<const A_u_char*>("Set Title")},
		{2, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	PT_MenuCmd_TITLE_LONGER,	reinterpret_cast<const A_u_char*>("Longer")},
		{2, AEGP_FlyoutMenuMarkType_NORMAL,		TRUE,	PT_MenuCmd_TITLE_SHORTER,	reinterpret_cast<const A_u_char*>("Shorter")},
	};
	A_long		menuTableSizeL = sizeof(myMenu) / sizeof(AEGP_FlyoutMenuItem);

#ifdef AE_OS_WIN // std::copy might be unsafe! Oh no! The sky is falling!
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif
	if (menuTableSizeL <= *in_out_numItemsP ){
		std::copy(&myMenu[0], &myMenu[menuTableSizeL], itemsP);
	} else {
		std::copy(&myMenu[0], myMenu + *in_out_numItemsP, itemsP);
	}
#ifdef AE_OS_WIN
	#pragma warning(pop)
#endif
	*in_out_numItemsP = menuTableSizeL;
}

void RenderbeamerUI::DoFlyoutCommand(AEGP_FlyoutMenuCmdID commandID)
{
	switch(commandID){
		case PT_MenuCmd_RED:
			i_use_bg = false;
			red = PF_MAX_CHAN8;
			green = blue = 0;
		break;
		case PT_MenuCmd_GREEN:
			i_use_bg = false;
			green = PF_MAX_CHAN8;
			red = blue = 0;
		break;
		case PT_MenuCmd_BLUE:
			i_use_bg = false;
			blue = PF_MAX_CHAN8;
			red = green = 0;
		break;
		case PT_MenuCmd_STANDARD:
			i_use_bg = true;
		break;

		case PT_MenuCmd_TITLE_LONGER:
			i_panelSuite->AEGP_SetTitle(i_panelH, reinterpret_cast<const A_u_char*>("This is a longer name"));
			break;

		case PT_MenuCmd_TITLE_SHORTER:
			i_panelSuite->AEGP_SetTitle(i_panelH, reinterpret_cast<const A_u_char*>("P!"));
			break;
	}
	InvalidateAll();
}

A_Err RenderbeamerUI::S_GetSnapSizes(AEGP_PanelRefcon refcon, A_LPoint*	snapSizes, A_long * numSizesP)
{
	PT_XTE_START{
		reinterpret_cast<RenderbeamerUI*>(refcon)->GetSnapSizes(snapSizes, numSizesP);
	} PT_XTE_CATCH_RETURN_ERR;
}

A_Err RenderbeamerUI::S_PopulateFlyout(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuItem* itemsP, A_long * in_out_numItemsP)
{
	PT_XTE_START{
		reinterpret_cast<RenderbeamerUI*>(refcon)->PopulateFlyout(itemsP, in_out_numItemsP);
	} PT_XTE_CATCH_RETURN_ERR;
}

A_Err RenderbeamerUI::S_DoFlyoutCommand(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuCmdID commandID)
{
	PT_XTE_START{
		reinterpret_cast<RenderbeamerUI*>(refcon)->DoFlyoutCommand(commandID);
	} PT_XTE_CATCH_RETURN_ERR;
}
