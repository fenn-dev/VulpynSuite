#include "application.h"
#include <imgui.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <Windows.h>
#include <cstdio>

namespace fs = std::filesystem;

namespace app {

    bool ShowFileSize;

    fs::path FocusedFile;

    fs::path rootFolder = "C:\\Users\\rasmu\\Documents\\TestingFolder\\"; //"C:/";

    // Check if a directory is accessible (non-throwing)
    bool IsDirectoryAccessible(const std::wstring& path) {
        HANDLE h = CreateFileW(
            path.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );
        if (h == INVALID_HANDLE_VALUE)
            return false;

        CloseHandle(h);
        return true;
    }

    void ParseFolderAndSubFolders(const fs::path& Path) {
        std::error_code ec;

        if (!fs::exists(Path, ec) || !fs::is_directory(Path, ec)) {
            ImGui::Text("Invalid root folder: %s", Path.string().c_str());
            return;
        }

        fs::directory_iterator dirIt(Path, ec);
        if (ec) return; // Skip if directory can't be opened

        for (const auto& entry : dirIt) {
            const fs::path& entryPath = entry.path();
            std::wstring wpath = entryPath.wstring();

            if (fs::is_directory(entryPath, ec)) {
                bool accessible = IsDirectoryAccessible(wpath);

                // Display folder name (red if inaccessible)
                if (accessible) {
                    if (ImGui::TreeNode(entryPath.filename().string().c_str())) {
                        ParseFolderAndSubFolders(entryPath);
                        ImGui::TreePop();
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        rootFolder = entryPath;
                        ParseFolderAndSubFolders(rootFolder);
                        return;
                    }
                }
                else {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s (Access Denied)", entryPath.filename().string().c_str());
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        // Optional: add your UAC elevation logic here if desired
                        std::cout << "Attempted access to restricted folder: " << entryPath << std::endl;
                    }
                }
            }
            else if (fs::is_regular_file(entryPath, ec)) {
                ImGui::Selectable(entryPath.filename().string().c_str());
                // Checks if the selectable has been right clicked
                if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    FocusedFile = entryPath;
                    ImGui::OpenPopup("FileProperties");
                }
            }
        }
    }

    bool openRenameModal = false; // global or static inside popups()

    void popups() {
        // Begins a popup
        if (ImGui::BeginPopup("FileProperties")) {
            ImGui::Button("Open");
            ImGui::Button("Open With...");
            ImGui::Separator();
            if (ImGui::Button("Rename")) {
                ImGui::CloseCurrentPopup();
                openRenameModal = true; // set flag to open modal in this frame
            }
            ImGui::Separator();
            ImGui::Button("Cut");
            ImGui::Button("Copy");
            ImGui::Button("Del");
            ImGui::EndPopup();
        }

        // After closing FileProperties popup, now call OpenPopup based on flag
        if (openRenameModal) {
            ImGui::OpenPopup("Rename File Modal");
            openRenameModal = false; // reset
        }

        // Now render modal
        static char newName[128] = "";
        static bool renameInitialized = false;

        if (ImGui::BeginPopupModal("Rename File Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            // Initialize newName only once per modal open
            if (!renameInitialized) {
                strncpy(newName, FocusedFile.filename().string().c_str(), sizeof(newName) - 1);
                newName[sizeof(newName) - 1] = '\0';
                renameInitialized = true;
            }

            ImGui::Text("Enter new name:");
            ImGui::InputText("##newname", newName, IM_ARRAYSIZE(newName));

            if (ImGui::Button("OK")) {
                fs::path newPath = FocusedFile.parent_path() / newName;
                if (std::rename(FocusedFile.string().c_str(), newPath.string().c_str()) == 0) {
                    std::cout << "File renamed successfully.\n";
                }
                else {
                    std::perror("Error renaming file");
                }
                ImGui::CloseCurrentPopup();
                renameInitialized = false; // Reset for next time
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                renameInitialized = false; // Reset for next time
            }

            ImGui::EndPopup();
        }

    }

    void FileManagerApp(PanelState& panel) {
        std::string windowTitle = "File Manager##" + std::to_string(panel.uid);

        if (ImGui::Begin(windowTitle.c_str(), &panel.open)) {
            ImGui::Text("Current Folder: %s", rootFolder.string().c_str());
            if (ImGui::Button("Go Up")) {
                if (rootFolder.has_parent_path()) {
                    rootFolder = rootFolder.parent_path();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Refresh")) {
                // Refresh logic can be added here
            }

            ImGui::SameLine();



            ParseFolderAndSubFolders(rootFolder);

            popups();
        }
        ImGui::End();
    }
}
