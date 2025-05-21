// File: FileIcon.h
#pragma once
#include <string>
#include <imgui.h>
#include <unordered_map>
#include <filesystem>


namespace iconhandler
{
    extern std::unordered_map<std::string, ImTextureID> iconMap;
    inline std::filesystem::path iconFolderPath = "C:\\Users\\rasmu\\Desktop\\VulpynSuite\\examples\\example_glfw_opengl3\\assets\\";

    void initIconMap();
    void displayICON(const std::string& fileType, float Scale = 24.0f);
}
