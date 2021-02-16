

#include "GF_AEGP_WindowsUI.h"
#include <stdio.h>

const char OSWndObjectProperty[] = "RenderbeamerUI_WinPtr";

const int BtnID = 444;


RenderbeamerUI_Win::RenderbeamerUI_Win(	SPBasicSuite* spbP, AEGP_PanelH panelH, 
										AEGP_PlatformViewRef platformWindowRef,
										AEGP_PanelFunctions1* outFunctionTable)
		: RenderbeamerUI(spbP, panelH, platformWindowRef, outFunctionTable)
		, i_prevWindowProc(NULL)
{
	// hook the main window
	i_prevWindowProc = (WindowProc)GetWindowLongPtr(platformWindowRef, GWLP_WNDPROC);
	// blasting the client's wndproc, it's required that they provide an adapter.

	SetWindowLongPtrA(platformWindowRef, GWLP_WNDPROC, (LONG_PTR)RenderbeamerUI_Win::StaticOSWindowWndProc);
	::SetProp(platformWindowRef, OSWndObjectProperty, (HANDLE)this);

	HWND btn = CreateWindow("BUTTON", "Collect & Send", 
							WS_CHILD | WS_VISIBLE| BS_CENTER | BS_VCENTER | BS_TEXT, 
							10, 200, 100, 30, 
							platformWindowRef, (HMENU)static_cast<long>(BtnID), NULL, NULL);
}

LRESULT CALLBACK RenderbeamerUI_Win::StaticOSWindowWndProc(	HWND	hWnd, 
															UINT	message, 
															WPARAM	wParam, 
															LPARAM	lParam)
{
	// suck out our window ptr
	RenderbeamerUI_Win* platPtr = reinterpret_cast<RenderbeamerUI_Win*>(::GetProp(hWnd, OSWndObjectProperty));
	
	if(platPtr) {
		return platPtr->OSWindowWndProc(hWnd, message, wParam, lParam);
	} else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

HBRUSH CreateBrushFromSelector(PFAppSuite6* appSuiteP, PF_App_ColorType sel)
{
	PF_App_Color	appColor = {0};
	appSuiteP->PF_AppGetColor(sel, &appColor);
	return CreateSolidBrush( RGB(appColor.red/255,appColor.green/255, appColor.blue/255));
}

void RenderbeamerUI_Win::DrawProgressBar(AEGP_PlatformViewRef winHandle, RECT drawRect)
{
	HBRUSH brush = NULL;
	RECT finR = { 0,0,0,0 }, ufinR = { 0,0,0,0 };
	HDC hdc = GetDC(winHandle);
	GetProgressBarRect(drawRect, finR, ufinR);

	brush = CreateBrushFromSelector(i_appSuite.get(), PF_App_Color_FILL_LIGHT);
	FillRect(hdc, &finR, brush);
	DeleteObject(brush);
	
	brush = CreateBrushFromSelector(i_appSuite.get(), PF_App_Color_HOT_TEXT);
	FillRect(hdc, &ufinR, brush);
	DeleteObject(brush);
}

LRESULT RenderbeamerUI_Win::OSWindowWndProc(HWND	hWnd, 
											UINT	message, 
											WPARAM	wParam, 
											LPARAM	lParam)
{
	LRESULT result = 0;
	// do I want to do something
	bool handledB = false;
	switch(message) 
	{
		case WM_PAINT: 
		{
			RECT clientArea, progressTextArea, progressBarArea, buttonArea;
			GetClientRect(hWnd, &clientArea);
			HBRUSH brush = NULL;
			HDC hdc = GetDC(hWnd);
			//AEFX_CLR_STRUCT(def)
			//PF_ADD_BUTTON
			//if(i_use_bg) {
			//	brush = CreateBrushFromSelector(i_appSuite.get(), PF_App_Color_PANEL_BACKGROUND);
			//} else {
			//	brush = CreateSolidBrush( RGB(red,green, blue));
			//}
			// PF_App_Color_HILITE
			brush = CreateBrushFromSelector(i_appSuite.get(), PF_App_Color_PANEL_BACKGROUND);
			FillRect(hdc, &clientArea, brush);
			DeleteObject(brush);	
			
			progressTextArea = clientArea;
			progressTextArea.bottom = progressTextArea.top + 40;

			progressBarArea = clientArea;
			progressBarArea.top = progressTextArea.bottom;
			progressBarArea.bottom = progressBarArea.top + 40;
			DrawProgressBar(hWnd, progressBarArea);
					   			

			char textToDraw[200];
			RECT origClientArea;
			GetClientRect(hWnd, &origClientArea);

			#ifdef AE_OS_WIN
				sprintf_s(textToDraw, "Size: %d, %d", origClientArea.right - origClientArea.left, origClientArea.bottom - origClientArea.top);
			#else
				sprintf(textToDraw, "Size: %d, %d", origClientArea.right - origClientArea.left, origClientArea.bottom - origClientArea.top);
			#endif
			buttonArea = progressBarArea;
			buttonArea.top = buttonArea.bottom;
			buttonArea.bottom = buttonArea.top + 30;
			
			DrawText(hdc, textToDraw, (uint32_t) strlen(textToDraw), &buttonArea, DT_SINGLELINE | DT_LEFT );
			
#ifdef AE_OS_WIN
			sprintf_s(textToDraw, "NumClicks: %d", i_numClicks);
#else
			sprintf(textToDraw, "NumClicks: %d", i_numClicks);
#endif
			clientArea.top = clientArea.bottom;
			clientArea.bottom = clientArea.top + 30;

			DrawText(hdc, textToDraw, (uint32_t) strlen(textToDraw), &clientArea, DT_SINGLELINE | DT_LEFT );
			
			handledB = true;
		}
		case WM_SIZING:
			InvalidateAll();
			break;

		case WM_COMMAND:
			if(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == BtnID){
				i_numClicks++;
				progressStep = i_numClicks;
				InvalidateAll();
				handledB = true;
			}	
		break;
	}

	if(i_prevWindowProc && !handledB) {
		result = CallWindowProc(i_prevWindowProc, hWnd, message, wParam, lParam);
	} else {
		result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;

}


void RenderbeamerUI_Win::InvalidateAll()
{
	RECT clientArea;
	GetClientRect(i_refH, &clientArea);

	InvalidateRect(i_refH, &clientArea, FALSE);
}
