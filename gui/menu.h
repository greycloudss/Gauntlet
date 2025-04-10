#pragma once

#include <windows.h>
#include <d3d11.h>

#include "..\inject\injector.h"
#include "..\disasm\dynAsm\disasm.h"
#include "..\disasm\statAsm\disasm.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace MENU {
    extern INJ::Injector*      injector;
    extern ASM::StatDisasm*    sAsm;
    extern ASM::DynDisasm*     dAsm;

    extern ID3D11Device* g_pd3dDevice;
    extern ID3D11DeviceContext* g_pd3dDeviceContext;
    extern IDXGISwapChain* g_pSwapChain;
    extern ID3D11RenderTargetView* g_mainRenderTargetView;

    void setObjects(INJ::Injector* a, ASM::StatDisasm* b, ASM::DynDisasm* c);

    void Render();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void InitImGui(HWND hwnd);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    int Main(HINSTANCE hInstance);
}
