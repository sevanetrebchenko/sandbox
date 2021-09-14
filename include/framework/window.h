
#ifndef SANDBOX_WINDOW_H
#define SANDBOX_WINDOW_H

#include <sandbox.h>
#include <framework/singleton.h>

namespace Sandbox {

    class Window : public Singleton<Window> {
        public:
            REGISTER_SINGLETON(Window);

            void Initialize() override;
            void Shutdown() override;

            void PollEvents();
            void SwapBuffers();
            bool IsActive();

            [[nodiscard]] GLFWwindow* GetNativeWindow() const;

            [[nodiscard]] int GetWidth() const;
            [[nodiscard]] int GetHeight() const;
            [[nodiscard]] const std::string& GetName() const;

        private:
            Window();
            ~Window() override;

            void InitializeGLFW();
            void CreateGLFWWindow();
            void SetupGLFWCallbacks();
            void InitializeOpenGLContext();
            void InitializeImGui();

            // Window data.
            GLFWwindow* _window;
            std::string _name;
            int _width;
            int _height;
    };

}

#endif // SANDBOX_WINDOW_H
