#pragma once
#include <d3d9.h>
#include "./FileSys/FileSys.h"


extern ImFont* iconFont;

namespace gui
{
    // Constant window size
    constexpr int WIDTH = 490; //325
    constexpr int HEIGHT = 265; //175

    // Global state variables
    inline bool isRunning = true;
    inline HWND window = nullptr;
    inline WNDCLASSEX windowClass = { };
    inline POINTS position = { };

    // DirectX state variables
    inline PDIRECT3D9 d3d = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline D3DPRESENT_PARAMETERS presentParameters = { };

    // Window management functions
    void CreateHWindow(const char* windowName) noexcept;
    void DestroyHWindow() noexcept;

    // Device management functions
    bool CreateDevice() noexcept;
    void ResetDevice() noexcept;
    void DestroyDevice() noexcept;

    // ImGui initialization and destruction
    void CreateImGui() noexcept;
    void DestroyImGui() noexcept;

    // Rendering functions
    void BeginRender() noexcept;
    void EndRender() noexcept;
    void Render() noexcept;

    // GUI functionalities
    void taskList() noexcept;
    void addTaskScreen() noexcept;

    //custom imgui elements
    void strikeThroughText(const std::string& text);
    bool strikeThroughButton(const std::string& label);
}