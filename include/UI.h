#pragma once
#include "stdafx.h"
#include <memory>
#include <string>
#include "imgui.h"

namespace HEIFtoJPEG
{
    struct UI
    {
        // Hardcoded UI values.
        const std::string xButtonLabel = "X##";
        const ImVec2 xButtonPadding = ImVec2(5, 5);
        const float xButtonRounding = 5.0f;
        const ImVec2 xButtonSize = ImVec2(23,25);

        GLuint fontTexture = 0;
        double mousePixels_x = 0;
        double mousePixels_y = 0;
        bool mousePressed[3] = { false,false,false };
        bool mouseReleased[3] = { false,false,false };
        bool windowClose = true;
        int window_width = 1280;
        int window_height = 720;

        // Progress bar values.
        ImVec2 progressBarSize = ImVec2(400.0, 50.0);
        ImVec2 progressBarPosition = ImVec2(0.0, 0.0);


        // Embedded icons for UI.
        ImTextureID fileID;
        ImTextureID helpIconID;
        ImTextureID homeID;
        ImTextureID upArrowID;
        unsigned int logoIcon = 0;
        std::vector<unsigned char> getDefaultFont();
       
        // Modal popups.
        ImVec2 fileDialogSize = ImVec2(800, 600);
        void FileDialogModalPopup();

        // Misc functions.
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void init();
        void render();
        void terminate();
        void SetDefaultStyle();

    private:
        void loadAllEmbeddedFiles();
        void renderUIHelper();
    };
}