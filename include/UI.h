#pragma once
#include "stdafx.h"
#include <memory>
#include "imgui.h"

namespace HEIFtoJPEG
{
    struct UI
    {
        double deltaX = 0.0;
        double deltaY = 0.0;
        int desktop_width = 1280;
        int desktop_height = 720;
        bool firstMouse = true;
        bool firstRun = true;
        GLuint fontTexture = 0;
        bool isInitialized = false;
        double lastX = 0;
        double lastY = 0;
        float lightRot = 0.0f;
        double mouseDeltaX = 0;
        double mouseDeltaY = 0;
        double mouseLastNDC_x=0;
        double mouseLastNDC_y=0;
        bool mouseLook = false;
        float mouseWheel = 0.0f;
        double mouseNDC_x=0;
        double mouseNDC_y=0;
        double mousePixels_x = 0;
        double mousePixels_y = 0;
        bool mousePressed[3] = { false,false,false };
        bool mouseReleased[3] = { false,false,false };
        bool showFrameCount = false;
        bool show_help_tooltips = false;
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
        void loadAllEmbeddedFiles();
       
        // Modal popups.
        bool showFileDialog = false;
        std::string fileDialogPath = "";
        ImVec2 fileDialogSize = ImVec2(800, 600);
        int fileLoadFlags = 0;
        void FileDialogModalPopup();

        // Misc functions.
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void init();
        void render();
        void terminate();
        void SetDefaultStyle();
        void TitleBarHelper();

    private:
        void renderUIHelper();
    };
}