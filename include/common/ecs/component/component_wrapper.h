
#pragma once

#include "pch.h"

namespace Sandbox {

    // Should not be stored across multiple frames.
    template <typename T>
    class ComponentWrapper {
        public:
            ComponentWrapper();
            explicit ComponentWrapper(T* component);
            ~ComponentWrapper();

            template <typename Fn>
            void Configure(Fn&& callback);

            T* Data() const;

            T* operator->() const;
            T& operator*() const;

            explicit operator bool() const;

        private:
            T* component_;
    };

}

#include "common/ecs/component/component_wrapper.tpp"