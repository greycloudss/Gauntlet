#include "menu.h"
#include <fstream>

namespace COLOUR {
    const char* userCfg = "userconfig.cfg";

    enum ThemeColour {
        windowBg,
        header,
        headerHovered,
        headerActive,
        button,
        buttonHovered,
        buttonActive,
        frameBg,
        frameBgHovered,
        frameBgActive,
        titleBg,
        titleBgActive,
        titleBgCollapsed,
        text,
        separator,
        Tab,
        TabHovered,
        TabActive,
        TabUnfocused,
        TabUnfocusedActive,
        ThemeColour_Count
    };
    
    ImVec4 themeDefaultColours[ThemeColour_Count] = {
        ImVec4(0.06f, 0.06f, 0.06f, 1.00f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.31f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.80f),
        ImVec4(0.26f, 0.59f, 0.98f, 1.00f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.40f),
        ImVec4(0.26f, 0.59f, 0.98f, 1.00f),
        ImVec4(0.06f, 0.53f, 0.98f, 1.00f),
        ImVec4(0.16f, 0.29f, 0.48f, 0.54f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.40f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.67f),
        ImVec4(0.04f, 0.04f, 0.04f, 1.00f),
        ImVec4(0.16f, 0.29f, 0.48f, 1.00f),
        ImVec4(0.00f, 0.00f, 0.00f, 0.51f),
        ImVec4(1.00f, 1.00f, 1.00f, 1.00f),
        ImVec4(0.43f, 0.43f, 0.50f, 0.50f),
        ImVec4(0.18f, 0.35f, 0.58f, 0.86f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.80f),
        ImVec4(0.20f, 0.41f, 0.68f, 1.00f),
        ImVec4(0.07f, 0.10f, 0.15f, 0.97f),
        ImVec4(0.14f, 0.26f, 0.42f, 1.00f)
    };

    ImVec4 themeColours[ThemeColour_Count] = {
        ImVec4(0.06f, 0.06f, 0.06f, 1.00f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.31f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.80f),
        ImVec4(0.26f, 0.59f, 0.98f, 1.00f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.40f),
        ImVec4(0.26f, 0.59f, 0.98f, 1.00f),
        ImVec4(0.06f, 0.53f, 0.98f, 1.00f),
        ImVec4(0.16f, 0.29f, 0.48f, 0.54f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.40f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.67f),
        ImVec4(0.04f, 0.04f, 0.04f, 1.00f),
        ImVec4(0.16f, 0.29f, 0.48f, 1.00f),
        ImVec4(0.00f, 0.00f, 0.00f, 0.51f),
        ImVec4(1.00f, 1.00f, 1.00f, 1.00f),
        ImVec4(0.43f, 0.43f, 0.50f, 0.50f),
        ImVec4(0.18f, 0.35f, 0.58f, 0.86f),
        ImVec4(0.26f, 0.59f, 0.98f, 0.80f),
        ImVec4(0.20f, 0.41f, 0.68f, 1.00f),
        ImVec4(0.07f, 0.10f, 0.15f, 0.97f),
        ImVec4(0.14f, 0.26f, 0.42f, 1.00f)
    };
    
    const char* themeColourNames[ThemeColour_Count] = {
        "Window BG",
        "Header",
        "Header Hovered",
        "Header Active",
        "Button",
        "Button Hovered",
        "Button Active",
        "Frame BG",
        "Frame Hovered",
        "Frame Active",
        "Title BG",
        "Title Active",
        "Title Collapsed",
        "Text",
        "Separator",
        "Tab",
        "Tab Hovered",
        "Tab Active",
        "Tab Unfocused",
        "Tab Unfocused Active"
    };
    
    ImVec4& get(ThemeColour colour) {
        return themeColours[colour];
    }

    void initColour() {
        std::ifstream input(userCfg);

        if (!input.is_open()) return;

        float r, g, b, a;
        for (int i = 0; i < ThemeColour_Count; ++i) {
            input >> r >> g >> b >> a;
            themeColours[i] = ImVec4(r, g, b, a);
        }

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
        style.Colors[ImGuiCol_Separator]            = COLOUR::get(COLOUR::separator);
        style.Colors[ImGuiCol_Tab]                  = COLOUR::get(COLOUR::Tab);
        style.Colors[ImGuiCol_TabHovered]           = COLOUR::get(COLOUR::TabHovered);
        style.Colors[ImGuiCol_TabActive]            = COLOUR::get(COLOUR::TabActive);
        style.Colors[ImGuiCol_TabUnfocused]         = COLOUR::get(COLOUR::TabUnfocused);
        style.Colors[ImGuiCol_TabUnfocusedActive]   = COLOUR::get(COLOUR::TabUnfocusedActive);
        input.close();
    }
    
    void writeColours() {
        std::ofstream output(userCfg);

        if (!output.is_open()) return;

        for (int i = 0; i < ThemeColour_Count; ++i) {
            const ImVec4& col = COLOUR::get(static_cast<COLOUR::ThemeColour>(i));
            output << col.x << ' ' << col.y << ' ' << col.z << ' ' << col.w << '\n';
        }
    }
};