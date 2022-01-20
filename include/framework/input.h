
#ifndef SANDBOX_INPUT_H
#define SANDBOX_INPUT_H

#include <sandbox_pch.h>

namespace Sandbox {

    class Input {
        public:
            static Input& Instance();

            [[nodiscard]] int GetKeyState(int key) const;
            [[nodiscard]] bool IsKeyPressed(int key) const;

            [[nodiscard]] int GetMouseButtonState(int button) const;
            [[nodiscard]] bool IsMouseButtonPressed(int button) const;

            [[nodiscard]] glm::vec2 GetMouseCursorPosition() const;

        private:
            Input();
            ~Input();
    };

}

#endif //SANDBOX_INPUT_H
