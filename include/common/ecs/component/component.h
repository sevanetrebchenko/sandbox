
#ifndef SANDBOX_COMPONENT_H
#define SANDBOX_COMPONENT_H

namespace Sandbox {

    // All components need to derive from IComponent.
    struct IComponent {
        virtual ~IComponent() = default;
    };

}

#endif //SANDBOX_COMPONENT_H
