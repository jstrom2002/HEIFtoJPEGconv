#include "stdafx.h" 
#include "UI.h"

int w = 800;
int h = 600;
int display_w, display_h;
ImFont* default_font;

namespace HEIFtoJPEG {
    std::shared_ptr<UI> ui = nullptr;
    GLFWwindow* window = nullptr;
    ImTextureID fileID;
    ImTextureID homeID;
    ImTextureID logoIcon;
    ImTextureID upArrowID;
    std::vector<std::string> to_load_filenames;
}

static void errorCallback(int code, const char* description) {
#ifdef _DEBUG
#ifdef _WIN32
    MessageBox(0, description, 0, 0);
#endif
    throw new std::exception(description);
#endif
}

static void resize_callback(GLFWwindow* win, int w, int h) {

    // bail if renderer is closing.
    if (!HEIFtoJPEG::ui)
        return;

    HEIFtoJPEG::ui->window_width = w;
    HEIFtoJPEG::ui->window_height = h;
    HEIFtoJPEG::ui->fileDialogSize = ImVec2(w * 0.75, h * 0.7);
}

bool initializeGLFW() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::exception("ERROR! Could not initialize the GLFW library.");
        return false;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor == nullptr) {
        throw std::exception("ERROR! Could not initialize the primary monitor.");
        return false;
    }
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode == nullptr) {
        throw std::exception("ERROR! Could not get the current monitor's video mode.");
        return false;
    }
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    //glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwSwapInterval(60); // 60 fps vsync.
    HEIFtoJPEG::window = glfwCreateWindow(w, h, "HEIF to JPEG", nullptr, nullptr);
    w = mode->width, h = mode->height;
    if (HEIFtoJPEG::window == nullptr) {
        throw std::exception("Failed to create window");
        glfwTerminate();
        return false;
    }
    glfwSetErrorCallback((GLFWerrorfun)errorCallback);
    glfwSetWindowSizeCallback(HEIFtoJPEG::window, (GLFWwindowsizefun)resize_callback);
    glfwMakeContextCurrent(HEIFtoJPEG::window);
    glfwSetWindowPos(HEIFtoJPEG::window, 0, 30);
    glfwSetInputMode(HEIFtoJPEG::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetFramebufferSize(HEIFtoJPEG::window, &display_w, &display_h);

    return true;
}
bool initializeGLEW() {
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        throw std::exception("Failed to initialize GLEW\n");
        glfwTerminate();
        return false;
    }

    return true;
}
bool initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(HEIFtoJPEG::window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;								   // Disable loading from .ini file.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
    glfwSetInputMode(HEIFtoJPEG::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    io.MouseDrawCursor = true;
    io.DisplaySize.x = static_cast<float>(w);
    io.DisplaySize.y = static_cast<float>(h);
    io.DisplayFramebufferScale.x = static_cast<float>(display_w) / w;
    io.DisplayFramebufferScale.y = static_cast<float>(display_h) / h;
    return true;
}
bool initializeOpenGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.3f, 0.3f, 0.3f, 0.f);
    glClearDepth(1.0f);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    return true;
}
bool initialize() {
    bool ret_val = true;
    ret_val &= initializeGLFW();
    ret_val &= initializeGLEW();
    ret_val &= initializeImGui();
    ret_val &= initializeOpenGL();
    return ret_val;
}
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(_WIN32)||defined(_WIN64)
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    HANDLE hMutex = CreateMutexA(NULL, FALSE, "3D Model Viewer Concurrency Prevention");
    DWORD dwMutexWaitResult = WaitForSingleObject(hMutex, 0);
    if (dwMutexWaitResult != WAIT_OBJECT_0) {
        MessageBox(HWND_DESKTOP, TEXT("This application is already running (only one app instance is allowed)."), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
        CloseHandle(hMutex);
        return -1;
    }
    
    if (!initialize())
        return -1;
    HEIFtoJPEG::ui = std::make_shared<HEIFtoJPEG::UI>();
    HEIFtoJPEG::ui->init();

    try {
        // Main loop.
        while (HEIFtoJPEG::window != nullptr && !glfwWindowShouldClose(HEIFtoJPEG::window))
        {
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            HEIFtoJPEG::ui->render();

            if (!glfwWindowShouldClose(HEIFtoJPEG::window))
                glfwSwapBuffers(HEIFtoJPEG::window);
        }

        // Shutdown and cleanup.
        HEIFtoJPEG::ui->terminate();
        glfwTerminate();
    }
    catch (std::exception e1)
    {
#ifdef _WIN32
        MessageBox(0, e1.what(), 0, 0);
#endif
    }
    ReleaseMutex(hMutex); // Explicitly release mutex
    CloseHandle(hMutex); // close handle before terminating
    return 0;
}