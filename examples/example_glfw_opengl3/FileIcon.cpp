#include "FileIcon.h"
#include <stb_image.h>
#include "ImageHandler.h"
#include <algorithm>

namespace iconhandler
{
    std::unordered_map<std::string, ImTextureID> iconMap;
    int ImgFileWidth = 0, ImgFileHeight = 0;

    void initIconMap()
    {
        iconMap.clear();
        for (const auto& entry : std::filesystem::directory_iterator(iconFolderPath))
        {
            if (entry.is_regular_file())
            {
                std::string stem = entry.path().stem().string(); // e.g. "cpp" from "cpp.png"
                std::transform(stem.begin(), stem.end(), stem.begin(), ::tolower);

                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
                {
                    GLuint iconTexture = LoadIconTexture(entry.path().string().c_str(), &ImgFileWidth, &ImgFileHeight);
                    iconMap[stem] = (ImTextureID)(intptr_t)iconTexture;
                }
            }
        }
    }

    void displayICON(const std::string& fileType, float Scale)
    {
        std::string key = fileType;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        auto it = iconMap.find(key);
        if (it != iconMap.end())
        {
            ImGui::Image(it->second, ImVec2(Scale, Scale));
        }
        else
        {
            // Fallback to "file" icon if available
            auto def = iconMap.find("file");
            if (def != iconMap.end())
                ImGui::Image(def->second, ImVec2(Scale, Scale));
            else
                ImGui::Text("Icon not found");
        }
    }
}
