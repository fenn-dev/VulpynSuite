#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <imgui.h>
#include <random>
#include <filesystem>
#include <vector>


namespace app {

    enum Panel {
        FileManager,
        SystemMonitor,
        TaskManager,
        Console,
        Log,
        Settings,
        DirTemplator
    };

    extern float iconW, iconH;
    extern ImTextureID FolderIcon;

    extern ImFont* Font_File_Explorer;
    inline bool g_NeedsFontReload = false;

    extern std::filesystem::path FocusedFile;

    struct PanelState {
        bool open = true;
        bool docked = false;
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        Panel panel = FileManager;
        int uid = rand() % 10000; // Unique ID for each panel
    };

    extern std::vector<PanelState> panels; // Declare as extern

    void SettingsApp(PanelState& panel);
    void FileManagerApp(PanelState& panel);
    void DirTemplatorApp(PanelState& panel);

    void startup();
    void ImGuiLoop();
    void ImGuiRenderLoop(GLFWwindow* window);
    void shutdown();
}
