
#ifndef GF_AEGP_MAINUI
#define GF_AEGP_MAINUI

#include "GF_GlobalTypes.h"

#ifndef AE_OS_WIN
typedef struct tagRECT
{
	A_long    left;
	A_long    top;
	A_long    right;
	A_long    bottom;
} RECT;
#endif

enum {
	PT_MenuCmd_RED = 30,
	PT_MenuCmd_GREEN,
	PT_MenuCmd_BLUE,
	PT_MenuCmd_STANDARD,
	PT_MenuCmd_TITLE_LONGER,
	PT_MenuCmd_TITLE_SHORTER
};

class RenderbeamerUI
{
public:
	explicit RenderbeamerUI(SPBasicSuite* spbP, AEGP_PanelH panelH, 
							AEGP_PlatformViewRef platformWindowRef,
							AEGP_PanelFunctions1* outFunctionTable);

protected:
	void operator=(const RenderbeamerUI&);
	RenderbeamerUI(const RenderbeamerUI&); // private, unimplemented

	AEGP_PlatformViewRef i_refH;
	AEGP_PanelH			i_panelH;

	A_u_char		red, green, blue;
	A_Boolean		i_use_bg;
	A_long			i_numClicks, progressStep, progressTotal;

	PF_FontName		appFontName;	
	A_short			appFontNum, appFontSize, appFontStyle;

	virtual void 	GetSnapSizes(A_LPoint*	snapSizes, A_long * numSizesP);
	virtual void	PopulateFlyout(AEGP_FlyoutMenuItem* itemsP, A_long * in_out_numItemsP);
	virtual void	DoFlyoutCommand(AEGP_FlyoutMenuCmdID commandID);
	virtual void	InvalidateAll() = 0;
	
	virtual void	DrawProgressBar(AEGP_PlatformViewRef winHandle, RECT drawRect) = 0;
	virtual void	GetProgressBarRect(RECT drawRect, RECT &finishedR, RECT &unfinishedR);

	SuiteHelper<PFAppSuite6>		i_appSuite;
	SuiteHelper<AEGP_PanelSuite1>	i_panelSuite;

private:
	static A_Err	S_GetSnapSizes(AEGP_PanelRefcon refcon, A_LPoint*	snapSizes, A_long * numSizesP);
	static A_Err	S_PopulateFlyout(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuItem* itemsP, A_long * in_out_numItemsP);
	static A_Err	S_DoFlyoutCommand(AEGP_PanelRefcon refcon, AEGP_FlyoutMenuCmdID commandID);

};

//////////////////////////////////////////////////////////////////////////////
#endif // RenderbeamerUI
