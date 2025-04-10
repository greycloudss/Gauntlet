#include "menu.h"
#include "colours.h"

namespace MENU {
    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    IDXGISwapChain* g_pSwapChain = nullptr;
    ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

    INJ::Injector* injector = nullptr;
    ASM::StatDisasm* sAsm = nullptr;
    ASM::DynDisasm* dAsm = nullptr;

    volatile char selected_tab = 0;

    void renderColourPickers() {
        ImGui::ColorEdit4("Window Background", (float*)&COLOUR::get(COLOUR::windowBg));
        ImGui::ColorEdit4("Header",            (float*)&COLOUR::get(COLOUR::header));
        ImGui::ColorEdit4("Header Hovered",    (float*)&COLOUR::get(COLOUR::headerHovered));
        ImGui::ColorEdit4("Header Active",     (float*)&COLOUR::get(COLOUR::headerActive));
        ImGui::ColorEdit4("Button",            (float*)&COLOUR::get(COLOUR::button));
        ImGui::ColorEdit4("Button Hovered",    (float*)&COLOUR::get(COLOUR::buttonHovered));
        ImGui::ColorEdit4("Button Active",     (float*)&COLOUR::get(COLOUR::buttonActive));
        ImGui::ColorEdit4("Frame Background",  (float*)&COLOUR::get(COLOUR::frameBg));
        ImGui::ColorEdit4("Frame Hovered",     (float*)&COLOUR::get(COLOUR::frameBgHovered));
        ImGui::ColorEdit4("Frame Active",      (float*)&COLOUR::get(COLOUR::frameBgActive));
        ImGui::ColorEdit4("Title Background",  (float*)&COLOUR::get(COLOUR::titleBg));
        ImGui::ColorEdit4("Title Active",      (float*)&COLOUR::get(COLOUR::titleBgActive));
        ImGui::ColorEdit4("Title Collapsed",   (float*)&COLOUR::get(COLOUR::titleBgCollapsed));
        ImGui::ColorEdit4("Text",              (float*)&COLOUR::get(COLOUR::text));
        ImGui::ColorEdit4("Separator",         (float*)&COLOUR::get(COLOUR::separator));

        if (ImGui::Button("Apply Theme")) {
            ImGuiStyle& style = ImGui::GetStyle();
            style.Colors[ImGuiCol_WindowBg]         = COLOUR::get(COLOUR::windowBg);
            style.Colors[ImGuiCol_Header]           = COLOUR::get(COLOUR::header);
            style.Colors[ImGuiCol_HeaderHovered]    = COLOUR::get(COLOUR::headerHovered);
            style.Colors[ImGuiCol_HeaderActive]     = COLOUR::get(COLOUR::headerActive);
            style.Colors[ImGuiCol_Button]           = COLOUR::get(COLOUR::button);
            style.Colors[ImGuiCol_ButtonHovered]    = COLOUR::get(COLOUR::buttonHovered);
            style.Colors[ImGuiCol_ButtonActive]     = COLOUR::get(COLOUR::buttonActive);
            style.Colors[ImGuiCol_FrameBg]          = COLOUR::get(COLOUR::frameBg);
            style.Colors[ImGuiCol_FrameBgHovered]   = COLOUR::get(COLOUR::frameBgHovered);
            style.Colors[ImGuiCol_FrameBgActive]    = COLOUR::get(COLOUR::frameBgActive);
            style.Colors[ImGuiCol_TitleBg]          = COLOUR::get(COLOUR::titleBg);
            style.Colors[ImGuiCol_TitleBgActive]    = COLOUR::get(COLOUR::titleBgActive);
            style.Colors[ImGuiCol_TitleBgCollapsed] = COLOUR::get(COLOUR::titleBgCollapsed);
            style.Colors[ImGuiCol_Text]             = COLOUR::get(COLOUR::text);
            style.Colors[ImGuiCol_Separator]        = COLOUR::get(COLOUR::separator);
        }
    }

    inline void injectorItems() {
        
    }

    inline void statAsmItems() {
        
    }

    uintptr_t decodeAddress(const char* input) {
        return static_cast<uintptr_t>(std::strtoull(input, nullptr, 0));
    }

    enum class ValueType {
        Int,
        Double,
        String,
        UintPtr,
        Count
    };
    
    inline const char* valueTypeNames[] = {
        "int",
        "double",
        "string",
        "uintptr_t"
    };
    
    ValueType selectedType = ValueType::Int;
    
    void renderTypeSelector() {
        int current = static_cast<int>(selectedType);
        if (ImGui::Combo("Value Type", &current, valueTypeNames, static_cast<int>(ValueType::Count))) {
            selectedType = static_cast<ValueType>(current);
        }
    }

    void convertValue(const char* inAddy, const char* inVal) {
        switch (selectedType) {
            case ValueType::Int: 
                dAsm->wpm<int>(decodeAddress(inAddy), std::stoi(inVal));
                break;
            case ValueType::Double: 
                dAsm->wpm<double>(decodeAddress(inAddy), std::stod(inVal));
                break;
            case ValueType::String: 
                dAsm->wpm<std::string>(decodeAddress(inAddy), std::string(inVal));
                break;
            case ValueType::UintPtr:
                dAsm->wpm<uintptr_t>(decodeAddress(inAddy), decodeAddress(inVal));
                break;
        }
    }
    

    inline void dynAsmItems() {
        static char addy[1024] = {};
        static char valy[1024] = {};
        ImGui::Text("Write Process Memory");
        ImGui::InputText("Address", addy, 1024, 0, (ImGuiInputTextCallback)__null, (void*)__null);
        ImGui::InputText("Value", valy, 1024, 0, (ImGuiInputTextCallback)__null, (void*)__null);

        renderTypeSelector();

        if (ImGui::Button("Write Memory")) convertValue(addy, valy);

        const char* items[] = { "TEST","TEST","TEST","TEST","TEST","TEST","TEST" };
        static int current_item = 0;
        
        ImGui::ListBox("##list", &current_item, items, IM_ARRAYSIZE(items), 6);
    }

    inline void tabs() {
        if (ImGui::BeginTabBar("Tabs")) {
            if (ImGui::BeginTabItem("Static Disasm")) {
                selected_tab = 0;

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Dynamic Disasm")) {
                selected_tab = 1;

                dynAsmItems();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Injector")) {
                selected_tab = 2;

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc")) {
                selected_tab = 3;


                renderColourPickers();


                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    void setObjects(INJ::Injector* a, ASM::StatDisasm* b, ASM::DynDisasm* c) {
        injector = a;
        dAsm = c;
        sAsm = b;
    }

    void Render() {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
        ImGui::Begin("Gauntlet Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        //items to be rendered

        tabs();
        
        ImGui::End();
    }

    void CreateRenderTarget() {
        ID3D11Texture2D* pBackBuffer = nullptr;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }

    void CleanupRenderTarget() {
        if (g_mainRenderTargetView) {
            g_mainRenderTargetView->Release();
            g_mainRenderTargetView = nullptr;
        }
    }

    void InitImGui(HWND hwnd) {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, 1,
            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, nullptr, &g_pd3dDeviceContext);

        CreateRenderTarget();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
        ImGui::StyleColorsDark();
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    int Main(HINSTANCE hInstance) {
        const wchar_t* className = L"MyWin";
        const wchar_t* windowTitle = L"Gauntlet";

        WNDCLASSEXW wc = {
            sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0, 0,
            hInstance, nullptr, nullptr, nullptr, nullptr,
            className, nullptr
        };

        RegisterClassExW(&wc);

        HWND hwnd = CreateWindowW(
            className,
            windowTitle,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            100, 100, 1280, 800,
            nullptr, nullptr,
            wc.hInstance,
            nullptr
        );

        InitImGui(hwnd);
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);

        MSG msg = {};
        while (msg.message != WM_QUIT) {
            if (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
                continue;
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            Render();

            ImGui::Render();
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            const float clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(1, 0);
        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupRenderTarget();
        g_pSwapChain->Release();
        g_pd3dDevice->Release();
        g_pd3dDeviceContext->Release();
        DestroyWindow(hwnd);
        UnregisterClassW(className, hInstance);
        return 0;
    }
}