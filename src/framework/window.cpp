
#include <framework/window.h>
#include <framework/camera.h>
#include <framework/backend.h>
#include <framework/imgui_log.h>

namespace Sandbox {

    Window::Window(std::string name, int width, int height) : _window(nullptr),
                                                              _name(std::move(name)),
                                                              _width(width),
                                                              _height(height)
                                                              {
        // Initialize GLFW.
        InitializeGLFW();
        CreateGLFWWindow();
        SetupGLFWCallbacks();

        // Initialize OpenGL.
        InitializeOpenGLContext();

        // Initialize ImGui.
        InitializeImGui();

        int size = 9;
        for (int i = size * size - 1; i >= 0; --i) {
            std::cout << std::setw(2) << i << " ";

            if (i % size == 0) {
                std::cout << std::endl;
            }
        }
    }

    Window::~Window() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void Window::InitializeGLFW() {
        int initializationCode = glfwInit();

        // Failed to initialize GLFW.
        if (!initializationCode) {
            throw std::runtime_error("Failed to initialize GLFW.");
        }
    }

    void Window::CreateGLFWWindow() {
        // Setting up OpenGL properties
        glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);

        // Failed to create GLFW window.
        if (!_window) {
            throw std::runtime_error("Failed to create GLFW window.");
        }
    }

    void Window::InitializeOpenGLContext() {
        // Initialize OpenGL.
        glfwMakeContextCurrent(_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize Glad (OpenGL).");
        }

        // OpenGL properties.
        Backend::Core::EnableFlag(GL_DEPTH_TEST);
//        Backend::Core::EnableFlag(GL_CULL_FACE);
//        Backend::Core::CullFace(GL_BACK);

        ImGuiLog& log = ImGuiLog::GetInstance();
        log.LogTrace("Vendor: %s", (const char*)glGetString(GL_VENDOR));
        log.LogTrace("Renderer: %s", (const char*)glGetString(GL_RENDERER));
        log.LogTrace("OpenGL Version: %s", (const char*)glGetString(GL_VERSION));
    }

    bool Window::IsActive() {
        return (glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(_window) == 0);
    }

    void Window::InitializeImGui() {
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
        ImGui_ImplGlfw_InitForOpenGL(_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    GLFWwindow *Window::GetNativeWindow() const {
        return _window;
    }

    void Window::SetupGLFWCallbacks() {
        glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xpos, double ypos) {
            Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));

            // Only update look at direction if the cursor is not showing.
            if (!camera->GetCursorEnabled()) {
                glm::vec2& mousePosition = camera->GetLastMousePosition();
                float xoffset = (float)xpos - mousePosition.x;
                float yoffset = mousePosition.y - (float)ypos;
                mousePosition.x = (float)xpos;
                mousePosition.y = (float)ypos;

                xoffset *= camera->GetCameraSensitivity();
                yoffset *= camera->GetCameraSensitivity();

                float& yaw = camera->GetCameraYaw();
                float& pitch = camera->GetCameraPitch();

                yaw += xoffset;
                pitch += yoffset;

                if(pitch > 89.0f)
                    pitch = 89.0f;
                if(pitch < -89.0f)
                    pitch = -89.0f;

                glm::vec3 direction;
                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                camera->SetLookAtDirection(glm::normalize(direction));
            }

        });

        glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
            bool& showCursor = camera->GetCursorEnabled();

            if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
                showCursor = !showCursor;

                if (showCursor) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
            }
        });
    }

    int Window::GetWidth() const {
        return _width;
    }

    int Window::GetHeight() const {
        return _height;
    }

    const std::string &Window::GetName() const {
        return _name;
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(_window);
    }

}