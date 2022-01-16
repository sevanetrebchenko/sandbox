
#ifndef SANDBOX_WINDOW_H
#define SANDBOX_WINDOW_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Window {
        public:
            Window(const std::string& name, int width, int height);
            ~Window();

            bool IsActive();

            void PollEvents();
            void SwapBuffers();

            // Calls to GetWidth / GetHeight will always return most up-to-date window size.
            [[nodiscard]] bool CheckForResize();

            void SetName(const std::string& name);

            [[nodiscard]] GLFWwindow* GetNativeWindow() const;
            [[nodiscard]] int GetWidth() const;
            [[nodiscard]] int GetHeight() const;

        private:
            // Window data.
            GLFWwindow* window_;
            int width_;
            int height_;
    };

}

#endif // SANDBOX_WINDOW_H
