#include "UI.h"
#include "CustomFileDialog.h"
#include "stdafx.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui_internal.h"
#include <filesystem>
#include "heif_converter.h"

namespace HEIFtoJPEG 
{

    void UI::SetDefaultStyle(){
        ImGuiStyle* style = &ImGui::GetStyle();
        style->Alpha = 1.000000;
        style->ChildRounding = 5.000000;
        style->ChildBorderSize = 2.000000;
        style->PopupRounding = 5.000000;
        style->PopupBorderSize = 2.000000;
        style->FramePadding = ImVec2(10, 10);
        style->FrameRounding = 5.000000;
        style->FrameBorderSize = 1.000000;
        style->ItemSpacing = ImVec2(10, 14);
        style->ItemInnerSpacing = ImVec2(10, 3);
        style->CellPadding = ImVec2(4, 4);
        style->TouchExtraPadding = ImVec2(0, 0);
        style->IndentSpacing = 26.000000;
        style->ColumnsMinSpacing = 6.000000;
        style->ScrollbarSize = 14.000000;
        style->ScrollbarRounding = 5.000000;
        style->GrabMinSize = 9.000000;
        style->GrabRounding = 2.000000;
        style->TabRounding = 5.000000;
        style->TabBorderSize = 1.000000;
        //style->TabMinWidthForUnselectedCloseButton = 0.000000;
        style->ColorButtonPosition = 1;
        style->ButtonTextAlign = ImVec2(0.5, 0.5);
        style->SelectableTextAlign = ImVec2(0, 0);
        style->DisplayWindowPadding = ImVec2(19, 19);
        style->DisplaySafeAreaPadding = ImVec2(15, 0);
        style->MouseCursorScale = 1.000000;
        style->AntiAliasedLines = 1;
        style->AntiAliasedFill = 1;
        style->CurveTessellationTol = 1.250000;
        //style->CircleSegmentMaxError = 1.600000;
        style->WindowPadding = ImVec2(5, 15);
        style->WindowRounding = 5.000000;
        style->WindowBorderSize = 2.000000;
        style->WindowMinSize = ImVec2(32, 32);
        style->WindowTitleAlign = ImVec2(0, 0.5);
        style->WindowMenuButtonPosition = 0;
        style->Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.6, 0.6, 0.6, 1);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.94, 0.94, 0.94, 0.86);
        style->Colors[ImGuiCol_ChildBg] = ImVec4(0.8, 0.796078, 0.807843, 0.501961);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(1, 1, 1, 0.6);
        style->Colors[ImGuiCol_Border] = ImVec4(0.925, 0.3725, 0.6078, 0.35);
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.925, 0.3725, 0.6078, 0.35);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.258824, 0.588235, 0.588235, 0.4);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.211765, 0.65098, 0.678431, 0.670588);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98, 0.98, 0.98, 0.53);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69, 0.69, 0.69, 0.8);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49, 0.49, 0.49, 0.8);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49, 0.49, 0.49, 1);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.203922, 0.780392, 0.780392, 1);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.203922, 0.745098, 0.776471, 0.780392);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.231373, 0.541176, 0.694118, 0.6);
        style->Colors[ImGuiCol_Button] = ImVec4(0.207843, 0.737255, 0.756863, 0.4);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.258824, 0.588235, 0.588235, 1);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.0980392, 0.529412, 0.541176, 1);
        style->Colors[ImGuiCol_Header] = ImVec4(0.14902, 0.588235, 0.596078, 0.309804);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26, 0.80, 0.75, 0.6);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.26, 0.89, 0.82, 1);
        style->Colors[ImGuiCol_Separator] = ImVec4(0.0705882, 0.380392, 0.34902, 0.619608);
        style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14, 0.44, 0.8, 0.78);
        style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.14, 0.44, 0.8, 1);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.8, 0.8, 0.8, 0.56);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26, 0.59, 0.98, 0.67);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26, 0.59, 0.98, 0.95);
        style->Colors[ImGuiCol_Tab] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TabHovered] = ImVec4(0.0980392, 0.541176, 0.552941, 0.8);
        style->Colors[ImGuiCol_TabActive] = ImVec4(0.431373, 0.694118, 0.698039, 1);
        style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.486275, 0.92549, 0.933333, 0.984314);
        style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.466667, 0.919608, 0.913725, 1);
        style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.678431, 0.796078, 0.835294, 0.929412);
        style->Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0980392, 0.541176, 0.552941, 0.8);
        style->Colors[ImGuiCol_TableRowBg] = ImVec4(0.431373, 0.694118, 0.698039, 1);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.39, 0.39, 0.39, 1);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1, 0.43, 0.35, 1);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9, 0.7, 0, 1);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1, 0.45, 0, 1);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.258824, 0.823529, 0.796078, 0.34902);
        style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.26, 0.59, 0.98, 0.95);
        style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.26, 0.59, 0.98, 0.8);
        style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.7, 0.7, 0.7, 0.7);
        style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.2, 0.2, 0.2, 0.2);
        style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2, 0.2, 0.2, 0.35);
    }
    void UI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        auto& io = ImGui::GetIO();
        io.MouseClickedPos[button] = io.MousePos;
    }
    void UI::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset){
        ui->mouseWheel += static_cast<float>(yoffset);
    }
    void deleteTextureHelper(ImTextureID tx_){
        GLuint tx = (GLuint)tx_;
        if(tx>0)
            glDeleteTextures(1, &tx);
    }
    void UI::terminate(){
        try{
            deleteTextureHelper(fileID);
            deleteTextureHelper(homeID);
            deleteTextureHelper(upArrowID);
            if (logoIcon)
                glDeleteTextures(1, &logoIcon);

            if (fontTexture)
            {
                glDeleteTextures(1, &fontTexture);
                ImGui::GetIO().Fonts->TexID = 0;
                fontTexture = 0;
            }

            ImGui_ImplGlfw_Shutdown();
            ImGui_ImplOpenGL3_Shutdown();
        }
        catch (std::exception e1){
            //ErrorMessageBox(e1.what());
        } 
    }
	void UI::init()
	{
        if (isInitialized)
            return;

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;								   // Disable loading from .ini file.
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        io.MouseDrawCursor = true;
        unsigned char* pixels;
        int width, height;
        std::vector<unsigned char> fnt = ui->getDefaultFont();
        io.Fonts->AddFontFromMemoryTTF(fnt.data(), fnt.size(), 14.0f);
        HEIFtoJPEG::CustomFileDialog::Instance()->runAfterLoadingAFont();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        io.Fonts->TexID = (void*)(intptr_t)fontTexture;
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
#ifdef _WIN32
        io.ImeWindowHandle = glfwGetWin32Window(window);
#endif
        //if (instantCallbacks)
        {
            //glfwSetCursorPosCallback(window, (GLFWcursorposfun)cursorCallback);
            glfwSetMouseButtonCallback(window, MouseButtonCallback);
            glfwSetScrollCallback(window, ScrollCallback);
            //glfwSetKeyCallback(window, KeyCallback);
            //glfwSetCharCallback(window, CharCallback);
        }
        int w, h, display_w, display_h;
        glfwGetWindowSize(window, &w, &h);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        io.DisplaySize.x = static_cast<float>(w);
        io.DisplaySize.y = static_cast<float>(h);
        io.DisplayFramebufferScale.x = static_cast<float>(display_w) / w;
        SetDefaultStyle();
        loadAllEmbeddedFiles();
        isInitialized = true;
	}
    void UI::render(){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        renderUIHelper();
        ImGui::EndFrame();
        if (window) {
            ImGui::Render();
            ImDrawData* imdata = ImGui::GetDrawData();
            ImGui_ImplOpenGL3_RenderDrawData(imdata);
        }
    }
    void UI::renderUIHelper()
    {
        static auto& io = ImGui::GetIO();

        if (glfwGetWindowAttrib(window, GLFW_RESIZABLE))
        {
            // setup display size (every frame to accommodate for window resizing)
            static int w, h, display_w, display_h;
            glfwGetWindowSize(window, &w, &h);
            glfwGetFramebufferSize(window, &display_w, &display_h);
            io.DisplaySize.x = static_cast<float>(w);
            io.DisplaySize.y = static_cast<float>(h);;
            io.DisplayFramebufferScale.x = static_cast<float>(display_w) / w;
            io.DisplayFramebufferScale.y = static_cast<float>(display_h) / h;
        }


        // setup time step
        static double current_time = glfwGetTime();
        io.DeltaTime = glm::abs(current_time - glfwGetTime());
        current_time = glfwGetTime();

        // Get mouse position in screen coordinates
        glfwGetCursorPos(window, &mousePixels_x, &mousePixels_y);
        io.MousePosPrev = io.MousePos;
        io.MousePos = ImVec2(static_cast<float>(mousePixels_x),
            static_cast<float>(this->mousePixels_y));

        //// NEW CODE -- Use ImGui window space coordinates.
        this->mouseNDC_x = ((this->mousePixels_x / window_width) - 0.5) * 2.0;
        this->mouseNDC_y = -((this->mousePixels_y / window_height) - 0.5) * 2.0;

        // Get mouse delta.
        this->mouseDeltaX = this->mouseNDC_x - this->mouseLastNDC_x;
        this->mouseDeltaY = this->mouseNDC_y - this->mouseLastNDC_y;

        for (auto i = 0; i < 3; i++)
        {
            io.MouseDown[i] = this->mousePressed[i] ||
                glfwGetMouseButton(window, i) != 0;
            // If a mouse press event came, always pass it as
            // "this frame", so we don't miss click-release events
            // that are shorter than 1 frame.
            this->mousePressed[i] = false;
        }

        io.MouseWheel = this->mouseWheel;
        //this->mouseWheel = 0.0f;
       
        // ========================================================

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
        bool dummyVal = true;
        if (ImGui::Begin("To Convert##file-to-load-child", &dummyVal, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration))
        {
            ImGui::BeginChild("##filepath_combo", ImVec2(window_width * 0.8f, 400), true);
            for (int i = 0; i < to_load_filenames.size(); ++i)
            {
                if (ImGui::Button((std::string("X##") + to_load_filenames[i]).c_str()))
                {
                    to_load_filenames.erase(to_load_filenames.begin() + i);
                    --i;
                    continue;
                }
                ImGui::SameLine();
                ImGui::Text(to_load_filenames[i].c_str());
            }
            ImGui::EndChild();



            static int out_format_choice = 0;
            static const char* current_out_format = "JPEG";
            static float quality_ = 90;
            static const float default_width_ = 90.0f;

            ImGui::SetNextItemWidth(default_width_);
            ImGui::SliderFloat("Quality##ui-q-slider", &quality_, 0.0f,100.0f);

            const char* items[] = { "JPEG", "PNG" };
            ImGui::SameLine();
            ImGui::SetNextItemWidth(default_width_);
            if (ImGui::BeginCombo("##output_type_combo", current_out_format))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    bool is_selected = (current_out_format == items[n]);
                    if (ImGui::Selectable(items[n], is_selected)) {
                        current_out_format = items[n];
                        out_format_choice = n;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }



            if (ImGui::Button("Browse##file-dialog"))
            {
                CustomFileDialog::Instance()->OpenDialog(
                    "Load File",
                    "Load File",
                    this->fileDialogSize,
                    this->fileDialogPath);
            }
            ImGui::SameLine();
            if (ImGui::Button("Convert")) {
                std::shared_ptr<HEIFtoJPEG::heif_converter> encoder = std::make_shared<HEIFtoJPEG::heif_converter>();
                for (auto f_ : to_load_filenames)
                {
                    encoder->Convert(f_.c_str(), out_format_choice+1, quality_);
                }
                to_load_filenames.clear();
            }



            ImGui::End();
        }



        if (showFileDialog)
            FileDialogModalPopup();
    }
    void UI::FileDialogModalPopup()
    {
        ImGui::SetNextWindowPos(ImVec2(
            0.5f * (window_width - fileDialogSize.x),
            0.5f * (window_height - fileDialogSize.y)
        ));
        ImGui::SetNextWindowSize(fileDialogSize, ImGuiCond_Always);

        if (CustomFileDialog::Instance()->FileDialog("Load File", ImGuiWindowFlags_NoCollapse)) {
            if (CustomFileDialog::Instance()->IsOk == true) {
                try {
                    to_load_filenames.push_back(CustomFileDialog::Instance()->GetFilepathName());
                }
                catch (std::exception e1) {
                    //ErrorMessageBox(e1.what());
                }
            }
            CustomFileDialog::Instance()->CloseDialog("Load File");
        }
    }
}