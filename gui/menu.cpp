#include "menu.h"
#include "colours.h"

namespace MENU {
    ID3D11Device* g_pd3dDevice = nullptr;
    ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
    IDXGISwapChain* g_pSwapChain = nullptr;
    ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
;
    INJ::Injector* injector;
    ASM::StatDisasm* sAsm;
    ASM::DynDisasm* dAsm;

    volatile char selected_tab = 0;

    void renderColourPickers() { // feels weird and wrong to write this, at least out right doing what you need instead of a loop is faster
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::windowBg],                           (float*)&COLOUR::get(COLOUR::windowBg), 2);
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::header],                             (float*)&COLOUR::get(COLOUR::header));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::headerHovered],                      (float*)&COLOUR::get(COLOUR::headerHovered));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::headerActive],                       (float*)&COLOUR::get(COLOUR::headerActive));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::button],                             (float*)&COLOUR::get(COLOUR::button));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::buttonHovered],                      (float*)&COLOUR::get(COLOUR::buttonHovered));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::buttonActive],                       (float*)&COLOUR::get(COLOUR::buttonActive));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::frameBg],                            (float*)&COLOUR::get(COLOUR::frameBg));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::frameBgHovered],                     (float*)&COLOUR::get(COLOUR::frameBgHovered));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::frameBgActive],                      (float*)&COLOUR::get(COLOUR::frameBgActive));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::titleBg],                            (float*)&COLOUR::get(COLOUR::titleBg));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::titleBgActive],                      (float*)&COLOUR::get(COLOUR::titleBgActive));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::titleBgCollapsed],                   (float*)&COLOUR::get(COLOUR::titleBgCollapsed));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::text],                               (float*)&COLOUR::get(COLOUR::text));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::separator],                          (float*)&COLOUR::get(COLOUR::separator));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::Tab],                       (float*)&COLOUR::get(COLOUR::separator));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::TabHovered],                (float*)&COLOUR::get(COLOUR::separator));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::TabActive],                 (float*)&COLOUR::get(COLOUR::separator));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::TabUnfocused],              (float*)&COLOUR::get(COLOUR::separator));
        ImGui::ColorEdit4(COLOUR::themeColourNames[COLOUR::TabUnfocusedActive],        (float*)&COLOUR::get(COLOUR::separator));


        if (ImGui::Button("Apply Theme")) {
            ImGuiStyle& style = ImGui::GetStyle();
            style.Colors[ImGuiCol_WindowBg]             = COLOUR::get(COLOUR::windowBg);
            style.Colors[ImGuiCol_Header]               = COLOUR::get(COLOUR::header);
            style.Colors[ImGuiCol_HeaderHovered]        = COLOUR::get(COLOUR::headerHovered);
            style.Colors[ImGuiCol_HeaderActive]         = COLOUR::get(COLOUR::headerActive);
            style.Colors[ImGuiCol_Button]               = COLOUR::get(COLOUR::button);
            style.Colors[ImGuiCol_ButtonHovered]        = COLOUR::get(COLOUR::buttonHovered);
            style.Colors[ImGuiCol_ButtonActive]         = COLOUR::get(COLOUR::buttonActive);
            style.Colors[ImGuiCol_FrameBg]              = COLOUR::get(COLOUR::frameBg);
            style.Colors[ImGuiCol_FrameBgHovered]       = COLOUR::get(COLOUR::frameBgHovered);
            style.Colors[ImGuiCol_FrameBgActive]        = COLOUR::get(COLOUR::frameBgActive);
            style.Colors[ImGuiCol_TitleBg]              = COLOUR::get(COLOUR::titleBg);
            style.Colors[ImGuiCol_TitleBgActive]        = COLOUR::get(COLOUR::titleBgActive);
            style.Colors[ImGuiCol_TitleBgCollapsed]     = COLOUR::get(COLOUR::titleBgCollapsed);
            style.Colors[ImGuiCol_Text]                 = COLOUR::get(COLOUR::text);
            style.Colors[ImGuiCol_Tab]                  = COLOUR::get(COLOUR::Tab);
            style.Colors[ImGuiCol_TabHovered]           = COLOUR::get(COLOUR::TabHovered);
            style.Colors[ImGuiCol_TabActive]            = COLOUR::get(COLOUR::TabActive);
            style.Colors[ImGuiCol_TabUnfocused]         = COLOUR::get(COLOUR::TabUnfocused);
            style.Colors[ImGuiCol_TabUnfocusedActive]   = COLOUR::get(COLOUR::TabUnfocusedActive);
        }

        ImGui::SameLine();
        if (ImGui::Button("Save Theme")) COLOUR::writeColours();

        ImGui::SameLine();
        if (ImGui::Button("Load Theme")) COLOUR::initColour();
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
        Float,
        String,
        UintPtr,
        Count
    };
    
    inline const char* valueTypeNames[] = {
        "int",
        "float",
        "string",
        "uintptr_t"
    };
    
    
    ValueType writeSelectedType = ValueType::Int;
    ValueType readSelectedType = ValueType::Int;
    
    void renderTypeSelector(const char* name, ValueType& sel) {
        int current = static_cast<int>(sel);
        if (ImGui::Combo(name, &current, valueTypeNames, static_cast<int>(ValueType::Count))) {
            sel = static_cast<ValueType>(current);
        }
    }

    void writeConvertValue(const char* inAddy, const char* inVal) {
        switch (writeSelectedType) {
            case ValueType::Int: 
                dAsm->wpm<int>(decodeAddress(inAddy), std::stoi(inVal));
                break;
            case ValueType::Float: 
                dAsm->wpm<float>(decodeAddress(inAddy), std::stod(inVal));
                break;
            case ValueType::String: 
                dAsm->wpm<std::string>(decodeAddress(inAddy), std::string(inVal));
                break;
            case ValueType::UintPtr:
                dAsm->wpm<uintptr_t>(decodeAddress(inAddy), decodeAddress(inVal));
                break;
        }
    }
    
    std::string readValue(uintptr_t address) {
        if (!dAsm) return "nullptr";
    
        switch (readSelectedType) {
            case ValueType::String: {
                return dAsm->rpm<std::string>(address);
            }
    
            case ValueType::Float: {
                float val = dAsm->rpm<float>(address);
                char buf[64];
                std::snprintf(buf, sizeof(buf), "%f", val);
                return std::string(buf);
            }
    
            case ValueType::Int: {
                int val = dAsm->rpm<int>(address);
                char buf[32];
                std::snprintf(buf, sizeof(buf), "%d", val);
                return std::string(buf);
            }
    
            case ValueType::UintPtr: {
                uintptr_t val = dAsm->rpm<uintptr_t>(address);
                char buf[32];
                std::snprintf(buf, sizeof(buf), "0x%llX", static_cast<unsigned long long>(val));
                return std::string(buf);
            }
    
            default:
                return "unknown";
        }
    }

    inline void dynAsmItems() {
        static char addyW[1024] = {};
        static char valyW[1024] = {};

        if (!dAsm) return;

        ImGui::Text("Write Process Memory");
        ImGui::InputText("Write Address", addyW, 1024, 0, (ImGuiInputTextCallback)__null, (void*)__null);
        ImGui::InputText("Value", valyW, 1024, 0, (ImGuiInputTextCallback)__null, (void*)__null);
        renderTypeSelector("Write Value Type", writeSelectedType);

        if (ImGui::Button("Write Memory") && !std::empty(addyW) && !std::empty(valyW)) writeConvertValue(addyW, valyW);
        ImGui::NewLine();

        static char addyR[1024] = {};
        static char resultBuf[1024] = {};
        
        ImGui::Text("Read Process Memory");
        ImGui::InputText("Read Address", addyR, 1024, 0, nullptr, nullptr);
        renderTypeSelector("Read Value Type", readSelectedType);
        
        // Show buffer BEFORE button
        ImGui::InputText("##readonly", resultBuf, sizeof(resultBuf), ImGuiInputTextFlags_ReadOnly);
        
        if (ImGui::Button("Read Memory") && !std::empty(addyR)) {
            std::string val = readValue(decodeAddress(addyR));
            strncpy(resultBuf, val.c_str(), sizeof(resultBuf) - 1);
            resultBuf[sizeof(resultBuf) - 1] = '\0';
        }
        ImGui::NewLine();
        ImGui::NewLine();
        

        static std::vector<const char*> cStrs;

        std::lock_guard<std::mutex> l(dAsm->toPrintMutex);
        size_t n = dAsm->toPrint.size();
        cStrs.resize(n);
        if (n) {
            const std::vector<std::string>& src = dAsm->toPrint;
            for (size_t i = 0; i < n; ++i)
                cStrs[i] = src[i].c_str();
        }
        
        static int current_item = 0;
        
        ImGui::ListBox("##list", &current_item, cStrs.data(), (int)cStrs.size(), 25);
        
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
        
        std::vector<std::string> localPrint;
        {
            std::lock_guard<std::mutex> lock(dAsm->toPrintMutex);
            localPrint = dAsm->toPrint;
        }
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
        //std::exit(0);
        return 0;
    }
}