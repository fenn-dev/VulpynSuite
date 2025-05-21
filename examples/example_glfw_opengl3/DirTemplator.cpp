#include <string>

#include "application.h"

namespace app
{
    void DirTemplatorApp(PanelState& panel)
    {
        std::string windowTitle = "DirTemplator##" + std::to_string(panel.uid);

        if (ImGui::Begin(windowTitle.c_str(), &panel.open)) {



        }
        ImGui::End();
    }
}
