// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <Spore/App/Canvas.h>

//
// Exported Functions
//

void Initialize()
{
	App::Canvas* appCanvas = App::Canvas::Get();
	if (appCanvas == nullptr)
	{
		MessageBoxA(nullptr, "couldn't retrieve app canvas!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}

	HWND windowHandle      = appCanvas->GetWindow();
	HMONITOR monitorHandle = appCanvas->GetMonitor();

	LONG windowFlags = GetWindowLongA(windowHandle, GWL_STYLE);
	if (windowFlags == 0)
	{
		MessageBoxA(nullptr, "couldn't retrieve window flags!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}

	windowFlags &= ~WS_BORDER;
	windowFlags &= ~WS_CAPTION;

	LONG ret = SetWindowLongA(windowHandle, GWL_STYLE, windowFlags);
	if (ret == 0)
	{
		MessageBoxA(nullptr, "couldn't apply new window flags!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}
	
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	BOOL res = GetMonitorInfoA(monitorHandle, &monitorInfo);
	if (res == FALSE)
	{
		MessageBoxA(nullptr, "couldn't retrieve monitor information!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}

	const int width  = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	const int height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	res = SetWindowPos(windowHandle, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);
	if (res == FALSE)
	{
		MessageBoxA(nullptr, "couldn't set window position and size!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}
}

void Dispose()
{
	// This method is called when the game is closing
}

void AttachDetours()
{
	// Call the attach() method on any detours you want to add
	// For example: cViewer_SetRenderType_detour::attach(GetAddress(cViewer, SetRenderType));
}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

