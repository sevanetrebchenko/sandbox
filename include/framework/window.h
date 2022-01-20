
#ifndef SANDBOX_WINDOW_H
#define SANDBOX_WINDOW_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Window {
        public:
            static Window& Instance();

            void Init();
            void Shutdown();

            bool IsActive();

            void PollEvents();
            void SwapBuffers();

            // Calls to GetWidth / GetHeight will always return most up-to-date window size.
            [[nodiscard]] bool CheckForResize();

            [[nodiscard]] GLFWwindow* GetNativeWindow() const;
            [[nodiscard]] int GetWidth() const;
            [[nodiscard]] int GetHeight() const;
            [[nodiscard]] glm::ivec2 GetDimensions() const;

            void SetName(const std::string& name);

            void SetWidth(int width);
            void SetHeight(int height);
            void SetDimensions(glm::ivec2 dimensions);

        private:
            Window();
            ~Window();

            // Window data.
            GLFWwindow* window_;
            glm::ivec2 dimensions_;
            bool initialized_;
    };

}

#endif // SANDBOX_WINDOW_H
