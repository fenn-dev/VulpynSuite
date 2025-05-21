#include "application.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <nlohmann/json.hpp> // Include the JSON library
#include <fstream>           // For file I/O
#include <iostream>
#include "ImageHandler.h"
#include <filesystem>
#include "settings.h"

#include "FileIcon.h"
#include "imgui_impl_glfw.h"

namespace fs = std::filesystem;

namespace app {
    std::vector<PanelState> panels;
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Application windows

bool NewPanelPopup = false;

void SavePanelsToFile(const std::string& filename) {
    nlohmann::json json;

    // Serialize each panel in the vector
    for (const auto& panel : app::panels) {
        json.push_back({
            { "uid", panel.uid },
            { "open", panel.open },
            { "docked", panel.docked },
            { "dockspace_flags", panel.dockspace_flags },
            { "panel", panel.panel }
            });
    }

    // Write the JSON data to a file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << json.dump(4); // Pretty print with 4 spaces
        file.close();
        std::cout << "Panels saved to " << filename << std::endl;
    }
    else {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
    }
}

void LoadPanelsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        nlohmann::json json;
        file >> json; // Read the JSON data
        file.close();

        // Clear the existing panels
        app::panels.clear();

        // Deserialize each panel and add it to the vector
        for (const auto& panelJson : json) {
            app::panels.push_back({
                panelJson.at("open").get<bool>(),
                panelJson.at("docked").get<bool>(),
                panelJson.at("dockspace_flags").get<ImGuiDockNodeFlags>(),
                panelJson.at("panel").get<app::Panel>(),
                panelJson.at("uid").get<int>() // Deserialize UID
                });
        }
    }
}


void NavBar() {
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            NewPanelPopup = true; // Set the flag to open the popup
        }
        ImGui::MenuItem("Open", "Ctrl+O");
        ImGui::MenuItem("Save", "Ctrl+S");
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // Handle keyboard shortcut for Ctrl+N
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_N)) {
        NewPanelPopup = true; // Open the popup when Ctrl+N is pressed
    }

    // New Panel Popup
    if (NewPanelPopup) { // Check if the popup should be open
        if (ImGui::Begin("New Panel", &NewPanelPopup, ImGuiWindowFlags_NoDocking)) {
            ImGui::Text("Select a panel to create:");

            if (ImGui::Button("Settings")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::Settings });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
                return;
            }if (ImGui::Button("Console")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::Console });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
                return;
            }if (ImGui::Button("Log")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::Log });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
                return;
            }if (ImGui::Button("File Manager")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::FileManager });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
                return;
            }if (ImGui::Button("System Monitor")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::SystemMonitor });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
                return;
            }if (ImGui::Button("Task Manager")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::TaskManager });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
                return;
            }if (ImGui::Button("DirTemplator")) {
                app::panels.push_back({ true, false, ImGuiDockNodeFlags_None, app::DirTemplator });
                NewPanelPopup = false; // Close the popup
                ImGui::End(); // End the window only if it was successfully created
            }


            ImGui::End(); // End the window only if it was successfully created
        }
    }
}

void HandlePanels() {
    for (auto& panel : app::panels) {
        if (panel.open) {
            switch (panel.panel) {
            case app::FileManager:
                app::FileManagerApp(panel);
                break;
            case app::Settings:
                app::SettingsApp(panel);
                break;
            case app::DirTemplator:
                app::DirTemplatorApp(panel);

            default:
                break;
            }
        }
        else {
            // Remove closed panels from the vector
            app::panels.erase(
                std::remove_if(app::panels.begin(), app::panels.end(), [](const app::PanelState& p) { return !p.open; }),
                app::panels.end()
            );
        }
    }
}


namespace app {
    ImFont* font_body = nullptr;
    ImFont* Font_File_Explorer = nullptr;
    float iconW = 18.0f, iconH = 18.0f;
    int ImgFolderWidth = 0, ImgFolderHeight = 0;
    int ImgFileWidth = 0, ImgFileHeight = 0;
    GLuint GL_FolderIcon = 0;
    ImTextureID FolderIcon = 0;

    void startup() {
        LoadPanelsFromFile("panels.json");
        ImGuiIO& io = ImGui::GetIO();

        font_body = io.Fonts->AddFontFromFileTTF(settings::FileExplorer_Font.c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        if (!font_body) {
            std::cerr << "Failed to load font_body! Falling back to default font." << std::endl;
        }
        Font_File_Explorer = io.Fonts->AddFontFromFileTTF(settings::FileExplorer_Font.c_str(), settings::FileExplorer_FontSize, NULL, io.Fonts->GetGlyphRangesDefault());
        if (!Font_File_Explorer) {
            std::cerr << "Failed to load Font_File_Explorer! Falling back to default font." << std::endl;
        }

        GL_FolderIcon = LoadIconTexture("C:\\Users\\rasmu\\Desktop\\VulpynSuite\\examples\\example_glfw_opengl3\\assets\\folder.png", &ImgFolderWidth, &ImgFolderHeight);
        FolderIcon = (ImTextureID)(intptr_t)GL_FolderIcon;

        iconhandler::initIconMap();
    }

    void ImGuiLoop() {
        if (g_NeedsFontReload)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();
            font_body = io.Fonts->AddFontFromFileTTF(settings::FileExplorer_Font.c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesDefault());
            Font_File_Explorer = io.Fonts->AddFontFromFileTTF(settings::FileExplorer_Font.c_str(), settings::FileExplorer_FontSize, NULL, io.Fonts->GetGlyphRangesDefault());
            io.Fonts->Build();
            ImGui_ImplOpenGL3_DestroyFontsTexture();
            ImGui_ImplOpenGL3_CreateFontsTexture();
            iconhandler::initIconMap();
            g_NeedsFontReload = false;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (font_body)
            ImGui::PushFont(font_body);
        else
            ImGui::PushFont(ImGui::GetFont()); // fallback to default font

        ImGui::DockSpaceOverViewport(0, ImGui::GetWindowViewport());
        NavBar();
        HandlePanels();
        ImGui::ShowDemoWindow();
        ImGui::PopFont();
    }

    void ImGuiRenderLoop(GLFWwindow* window) {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void shutdown() {
        SavePanelsToFile("panels.json"); // Save panels to a filed
    }
}

