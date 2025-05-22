#include <string>

#include "application.h"
#include <imgui.h>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace app
{
    fs::path template_path = "C:\\Users\\rasmu\\Desktop\\VulpynSuite\\examples\\example_glfw_opengl3\\assets\\FolderTemplate";

    void templateCustomizer()
    {
        // MARGIN (top)
        ImGui::Dummy(ImVec2(0, 10)); // 10px top margin

        // Simulate a box with background color and padding
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.3f, 0.5f, 1.0f));
        ImGui::BeginChild("MyBox", ImVec2(300, 150), true);

        // PADDING (inside the box)
        ImGui::SetCursorPos(ImVec2(10, 10)); // 10px padding from top-left
        ImGui::Text("Hello inside the box!");
        ImGui::Button("Click Me");

        ImGui::EndChild();
        ImGui::PopStyleColor();

        // MARGIN (bottom)
        ImGui::Dummy(ImVec2(0, 10)); // 10px bottom margin
    }

    void TemplateBrowser()
    {
         // loop trough and find all files in the template_path
        try {
            for (const auto& entry : fs::directory_iterator(template_path)) {
                if (entry.is_regular_file()) {
                    if (ImGui::Button(entry.path().filename().string().c_str()))
                    {
                        FocusedFile = entry;
                    }
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << '\n';
        }
        catch (const std::exception& e) {
            std::cerr << "General error: " << e.what() << '\n';
        }
        
    }

    void DirTemplatorApp(PanelState& panel)
    {
        std::string windowTitle = "DirTemplator##" + std::to_string(panel.uid);

        if (ImGui::Begin(windowTitle.c_str(), &panel.open)) {

            TemplateBrowser();

        }
        ImGui::End();
    }
}
