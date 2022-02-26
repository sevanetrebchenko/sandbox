
#include "common/api/window.h"
#include "common/api/backend.h"
#include "common/utility/log.h"

namespace Sandbox {

    Window& Window::Instance() {
        // Scott Meyers singleton.
        static Window window;
        return window;
    }

    Window::Window() : window_(nullptr),
                       dimensions_(glm::ivec2(1280, 720)),
                       initialized_(false)
                       {
    }

    Window::~Window() {
    }

    void Window::Init() {
        // Initialize GLFW.
        int initializationCode = glfwInit();
        if (!initializationCode) {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        // Setting up OpenGL properties
        glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create window.
        window_ = glfwCreateWindow(dimensions_.x, dimensions_.y, "OpenGL Sandbox", nullptr, nullptr);
        if (!window_) {
            throw std::runtime_error("Failed to create GLFW window.");
        }

        // Initialize OpenGL.
        glfwMakeContextCurrent(window_);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize Glad (OpenGL).");
        }

        ImGuiLog& log = ImGuiLog::Instance();
        log.LogTrace("Vendor: %s", (const char*)glGetString(GL_VENDOR));
        log.LogTrace("Renderer: %s", (const char*)glGetString(GL_RENDERER));
        log.LogTrace("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        std::string fontFilepath = ConvertToNativeSeparators(GetWorkingDirectory() + "/assets/fonts/inconsolata/Inconsolata-Regular.ttf");
        io.FontDefault = io.Fonts->AddFontFromFileTTF(fontFilepath.c_str(), 17.0f);
        ImGui::StyleColorsDark();

        // Initialize ImGui Flags.
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Platform/Renderer backend.
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    void Window::Shutdown() {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    bool Window::IsActive() {
        return (glfwGetKey(window_, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(window_) == 0);
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(window_);
    }

    bool Window::CheckForResize() {
        int width;
        int height;
        glfwGetFramebufferSize(window_, &width, &height);

        bool resized = false;

        if (dimensions_.x != width || dimensions_.y != height) {
            dimensions_ = glm::ivec2(width, height);
            resized = true;
        }

        return resized;
    }

    GLFWwindow *Window::GetNativeWindow() const {
        return window_;
    }

    int Window::GetWidth() const {
        return dimensions_.x;
    }

    int Window::GetHeight() const {
        return dimensions_.y;
    }

    glm::ivec2 Window::GetDimensions() const {
        return dimensions_;
    }

    void Window::SetName(const std::string& name) {
        glfwSetWindowTitle(window_, name.c_str());
    }

    void Window::SetWidth(int width) {
        dimensions_.x = width;
    }

    void Window::SetHeight(int height) {
        dimensions_.y = height;
    }

    void Window::SetDimensions(glm::ivec2 dimensions) {
        dimensions_ = dimensions;
    }

}