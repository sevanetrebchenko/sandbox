
#pragma once

namespace Sandbox {

    template <typename T>
    ComponentWrapper<T>::ComponentWrapper() : component_(nullptr) {
    }

    template <typename T>
    ComponentWrapper<T>::ComponentWrapper(T* component) : component_(component) {
    }

    template <typename T>
    ComponentWrapper<T>::~ComponentWrapper<T>() {
    }

    template <typename T>
    template <typename Fn>
    void ComponentWrapper<T>::Configure(Fn&& callback) {
        callback(*component_);
    }

    template <typename T>
    T* ComponentWrapper<T>::Data() const {
        return component_;
    }

    template <typename T>
    T& ComponentWrapper<T>::operator*() const {
        return *component_;
    }

    template <typename T>
    T* ComponentWrapper<T>::operator->() const {
        return component_;
    }

    template <typename T>
    ComponentWrapper<T>::operator bool() const {
        return component_ != nullptr;
    }

}