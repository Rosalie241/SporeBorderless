// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//
// Internal Variables
//

static HWND l_SporeWindowHandle = nullptr;

//
// Internal Functions
//

static BOOL CALLBACK enum_windows_proc_callback(HWND hWnd, LPARAM lParam)
{
	if (l_SporeWindowHandle != nullptr)
	{
		return TRUE;
	}

	DWORD processId;
	HRESULT res = GetWindowThreadProcessId(hWnd, &processId);

	if (res != 0 &&
		processId == lParam)
	{
		// only match windows with SPORE in the name
		char windowTitle[256+1] = { 0 };

		GetWindowTextA(hWnd, windowTitle, 256);

		if (strstr(windowTitle, "SPORE") != nullptr)
		{
			l_SporeWindowHandle = hWnd;
		}
	}

	return TRUE;
}

//
// Exported Functions
//

void Initialize()
{
	EnumWindows(enum_windows_proc_callback, GetCurrentProcessId());
	if (l_SporeWindowHandle == nullptr)
	{
		MessageBoxA(nullptr, "couldn't find window handle!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}

	LONG windowFlags = GetWindowLongA(l_SporeWindowHandle, GWL_STYLE);
	if (windowFlags == 0)
	{
		MessageBoxA(nullptr, "couldn't retrieve window flags!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}

	windowFlags &= ~WS_BORDER;
	windowFlags &= ~WS_CAPTION;

	LONG ret = SetWindowLongA(l_SporeWindowHandle, GWL_STYLE, windowFlags);
	if (ret == 0)
	{
		MessageBoxA(nullptr, "couldn't apply new window flags!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}
	
	HMONITOR hMonitor = MonitorFromWindow(l_SporeWindowHandle, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFO hMonitorInfo;
	hMonitorInfo.cbSize = sizeof(MONITORINFO);
	BOOL res = GetMonitorInfoA(hMonitor, &hMonitorInfo);
	if (res == FALSE)
	{
		MessageBoxA(nullptr, "couldn't retrieve monitor information!", "SporeBorderless", MB_OK | MB_ICONERROR);
		return;
	}

	const int width  = hMonitorInfo.rcMonitor.right - hMonitorInfo.rcMonitor.left;
	const int height = hMonitorInfo.rcMonitor.bottom - hMonitorInfo.rcMonitor.top;
	res = SetWindowPos(l_SporeWindowHandle, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);
	if (res == FALSE)
	{
		MessageBoxA(nullptr, "couldn't refresh window to force window flag change!", "SporeBorderless", MB_OK | MB_ICONERROR);
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

