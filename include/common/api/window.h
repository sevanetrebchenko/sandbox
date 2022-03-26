
#ifndef SANDBOX_WINDOW_H
#define SANDBOX_WINDOW_H

#include "pch.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class Window : public ISingleton<Window> {
        public:
            REGISTER_SINGLETON(Window);

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
            ~Window() override;

            // Window data.
            GLFWwindow* window_;
            glm::ivec2 dimensions_;
    };

}

#endif // SANDBOX_WINDOW_H
