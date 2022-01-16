
#include <framework/window.h>
#include <framework/camera.h>
#include <framework/backend.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    Window::Window(const std::string& name, int width, int height) : window_(nullptr),
                                                                     width_(width),
                                                                     height_(height)
                                                                     {
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
        window_ = glfwCreateWindow(width_, height_, name.c_str(), nullptr, nullptr);
        if (!window_) {
            throw std::runtime_error("Failed to create GLFW window.");
        }

        // Initialize OpenGL.
        glfwMakeContextCurrent(window_);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize Glad (OpenGL).");
        }

        ImGuiLog& log = ImGuiLog::GetInstance();
        log.LogTrace("Vendor: %s", (const char*)glGetString(GL_VENDOR));
        log.LogTrace("Renderer: %s", (const char*)glGetString(GL_RENDERER));
        log.LogTrace("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/inconsolata/Inconsolata-Regular.ttf", 17.0f);
        ImGui::StyleColorsDark();

        // Initialize ImGui Flags.
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Platform/Renderer backend.
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    Window::~Window() {
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

        if (width_ != width || height_ != height) {
            // Resized.
            width_ = width;
            height_ = height;

            resized = true;
        }

        return resized;
    }

    void Window::SetName(const std::string& name) {
        glfwSetWindowTitle(window_, name.c_str());
    }

    GLFWwindow *Window::GetNativeWindow() const {
        return window_;
    }

    int Window::GetWidth() const {
        return width_;
    }

    int Window::GetHeight() const {
        return height_;
    }

}