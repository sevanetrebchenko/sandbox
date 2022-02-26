
#ifndef SANDBOX_INPUT_H
#define SANDBOX_INPUT_H

#include "pch.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class Input : public Singleton<Input> {
        public:
            REGISTER_SINGLETON(Input);

            [[nodiscard]] int GetKeyState(int key) const;
            [[nodiscard]] bool IsKeyPressed(int key) const;

            [[nodiscard]] int GetMouseButtonState(int button) const;
            [[nodiscard]] bool IsMouseButtonPressed(int button) const;

            [[nodiscard]] glm::vec2 GetMouseCursorPosition() const;

        private:
            Input();
            ~Input() override;
    };

}

#endif //SANDBOX_INPUT_H
