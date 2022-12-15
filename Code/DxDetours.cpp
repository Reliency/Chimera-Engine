#include "ScriptManager.hpp"
#include "Hooking.hpp"
#include "D3DRenderer.hpp"
#include "UI/UIManager.hpp"
using namespace Chimera;
HRESULT Hooks::Present(IDXGISwapChain* dis, UINT syncInterval, UINT flags)
{
	if (g_Running)
	{
		g_D3DRenderer->BeginFrame();
		g_UiManager->OnTick();
		g_D3DRenderer->EndFrame();
	}

	return g_Hooking->m_D3DHook.GetOriginal<decltype(&Present)>(PresentIndex)(dis, syncInterval, flags);
}

HRESULT Hooks::ResizeBuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
	if (g_Running)
	{
		g_D3DRenderer->PreResize();
		auto hr = g_Hooking->m_D3DHook.GetOriginal<decltype(&ResizeBuffers)>(ResizeBuffersIndex)(dis, bufferCount, width, height, newFormat, swapChainFlags);
		if (SUCCEEDED(hr))
		{
			g_D3DRenderer->PostResize();
		}

		return hr;
	}

	return g_Hooking->m_D3DHook.GetOriginal<decltype(&ResizeBuffers)>(ResizeBuffersIndex)(dis, bufferCount, width, height, newFormat, swapChainFlags);
}
