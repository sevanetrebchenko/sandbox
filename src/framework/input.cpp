
#include <framework/input.h>
#include <framework/window.h>

namespace Sandbox {

    Input& Input::Instance() {
        static Input instance;
        return instance;
    }

    Input::Input() {
    }

    Input::~Input() {
    }

    int Input::GetKeyState(int key) const {
        return glfwGetKey(Window::Instance().GetNativeWindow(), key);
    }

    bool Input::IsKeyPressed(int key) const {
        return GetKeyState(key) == GLFW_PRESS;
    }

    int Input::GetMouseButtonState(int button) const {
        return glfwGetMouseButton(Window::Instance().GetNativeWindow(), button);
    }

    bool Input::IsMouseButtonPressed(int button) const {
        return GetMouseButtonState(button) == GLFW_PRESS;
    }

    glm::vec2 Input::GetMouseCursorPosition() const {
        static glm::dvec2 mouseCursorPosition;
        glfwGetCursorPos(Window::Instance().GetNativeWindow(), &mouseCursorPosition.x, &mouseCursorPosition.y);
        return { mouseCursorPosition.x, mouseCursorPosition.y };
    }

}