#pragma once
#include "ScriptManager.hpp"
#include "Hooking.hpp"
#include "D3DRenderer.hpp"
#include "UI/UIManager.hpp"
using namespace Chimera;

LRESULT Hooks::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TRY_CLAUSE{
		if (g_Running)
		{
			g_D3DRenderer->WndProc(hWnd, msg, wParam, lParam);
		}
	return CallWindowProcW(g_Hooking->m_og_wndproc, hWnd, msg, wParam, lParam);
	} EXCEPT_CLAUSE
		return NULL;

}