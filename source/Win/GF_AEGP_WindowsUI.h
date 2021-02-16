#ifndef GF_AEGP_WINDOWSUI
#define GF_AEGP_WINDOWSUI

#include "..\GF_AEGP_MainUI.h"
#include "SPBasic.h"
#include "AEConfig.h"

class RenderbeamerUI_Win : public RenderbeamerUI
{
public:
	explicit RenderbeamerUI_Win(	SPBasicSuite* spbP,
								AEGP_PanelH panelH, 
								AEGP_PlatformViewRef platformWindowRef,
								AEGP_PanelFunctions1* outFunctionTable);


protected:
	virtual void InvalidateAll() override;
	virtual void DrawProgressBar(AEGP_PlatformViewRef winHandle, RECT drawRect) override;

private:
	void operator=(const RenderbeamerUI&);
	RenderbeamerUI_Win(const RenderbeamerUI_Win&); // private, unimplemented


	typedef LRESULT (CALLBACK* WindowProc)(	HWND	hWnd, 
		UINT	message, 
		WPARAM	wParam, 
		LPARAM	lParam);

	WindowProc							i_prevWindowProc;


	static LRESULT CALLBACK				StaticOSWindowWndProc(	HWND	hWnd, 
		UINT	message, 
		WPARAM	wParam, 
		LPARAM	lParam);


	LRESULT OSWindowWndProc(	HWND	hWnd, 
		UINT	message, 
		WPARAM	wParam, 
		LPARAM	lParam);

};

#endif