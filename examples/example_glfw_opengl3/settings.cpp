#include "settings.h"
#include <string>
#include "application.h"
#include <imgui.h>
#include "FileIcon.h"


namespace app
{

    void SettingsApp(PanelState& panel) {
        std::string windowTitle = "Settings##" + std::to_string(panel.uid);

        if (ImGui::Begin(windowTitle.c_str(), &panel.open)) {
            if (ImGui::SliderFloat("Font Size", &settings::FileExplorer_FontSize, 10.0f, 50.0f, "%.1f"))
            {
                g_NeedsFontReload = true; // Set the flag, do not reload font here!
            }

            //Imgui treenode
            if (ImGui::TreeNode("File Explorer Settings"))
            {
                ImGui::Checkbox("Show Hidden Files", &settings::FileExplorer_ShowHiddenFiles);
                ImGui::Checkbox("Show File Size", &settings::FileExplorer_ShowFileSize);
                ImGui::Checkbox("Show File Type", &settings::FileExplorer_ShowFileType);
                ImGui::Checkbox("Show File Date", &settings::FileExplorer_ShowFileDate);
                ImGui::Checkbox("Show File Icon", &settings::FileExplorer_ShowFileIcon);
                ImGui::Checkbox("Show Denied File", &settings::FileExplorer_ShowDeniedFile);
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}
