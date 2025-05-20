#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <imgui.h>
#include <random>

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

    extern int iconW, iconH;
    extern GLuint myIcon;
    extern ImTextureID FolderIcon;

    struct PanelState {
        bool open = true;
        bool docked = false;
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        Panel panel = FileManager;
        int uid = rand() % 10000; // Unique ID for each panel
    };

    extern std::vector<PanelState> panels; // Declare as extern

    void FileManagerApp(PanelState& panel);

    void startup();
    void ImGuiLoop();
    void ImGuiRenderLoop(GLFWwindow* window);
    void shutdown();
}
