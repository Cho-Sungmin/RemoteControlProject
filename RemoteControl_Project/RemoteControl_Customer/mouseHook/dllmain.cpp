// dllmain.cpp : DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include "stdio.h"
#include "rcprotocol.h"

HINSTANCE hInstance = NULL;
HHOOK hMouseHook = NULL;
HHOOK hKBHook = NULL;

HWND hwnd = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		hInstance = hModule;
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
LRESULT MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	Mouse_Point mp;
	CRect rc;
	double x, y;

	if (nCode == HC_ACTION)
	{
		if(wParam == WM_LBUTTONDBLCLK /*| wParam == WM_MOUSEMOVE*/ | wParam == WM_LBUTTONUP
			| wParam == WM_LBUTTONDOWN | wParam == WM_RBUTTONUP | wParam == WM_RBUTTONDOWN)
		{
			MOUSEHOOKSTRUCT *pParamStruct = (MOUSEHOOKSTRUCT*)lParam;
			hwnd = pParamStruct->hwnd;
			GetWindowRect(hwnd, &rc);
			x = pParamStruct->pt.x - rc.left;
			y = pParamStruct->pt.y - rc.top;

			GetClientRect(pParamStruct->hwnd, &rc);

			mp.point.x = x / rc.right;
			mp.point.y = y / rc.bottom;
			mp.msg = wParam;

			SendMessage(pParamStruct->hwnd, ON_MOUSEHOOK, 0, (LPARAM)&mp);
		}
		else if (wParam == WM_MOUSEWHEEL)
		{
			MOUSEHOOKSTRUCTEX *pParamStruct = (MOUSEHOOKSTRUCTEX *)lParam;

			mp.point.y = pParamStruct->mouseData;
			mp.point.x = 0;
			mp.msg = wParam;

			SendMessage(pParamStruct->hwnd, ON_MOUSEHOOK, 0, (LPARAM)&mp);
		}
	}

	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

LRESULT KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT *pParamStruct;

	if (nCode > 0) {
		if (!(lParam & 0x80000000) && hwnd != NULL) {
			pParamStruct = (KBDLLHOOKSTRUCT*)lParam;
			SendMessage(hwnd, ON_KBHOOK, wParam, 0);
		}
	}

	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) void mouseHookStart(DWORD hThread)
{
	hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, hInstance, hThread);
	if (hMouseHook == NULL)
	{
		FILE *fp;
		DWORD errNo = GetLastError();

		fopen_s(&fp, "mouseHookLog.txt", "a");

		fprintf(fp, "Error : %d\n", errNo);
		fclose(fp);
	}
}
extern "C" __declspec(dllexport) void mouseHookStop()
{
	UnhookWindowsHookEx(hMouseHook);
	hMouseHook = NULL;

}

extern "C" __declspec(dllexport) void kbHookStart(DWORD hThread)
{
	hKBHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInstance, hThread);
	if (hKBHook == NULL)
	{
		FILE *fp;
		DWORD errNo = GetLastError();

		fopen_s(&fp, "keybdHookLog.txt", "a");

		fprintf(fp, "Error : %d\n", errNo);
		fclose(fp);
	}
}
extern "C" __declspec(dllexport) void kbHookStop()
{
	UnhookWindowsHookEx(hKBHook);
	hKBHook = NULL;

}
