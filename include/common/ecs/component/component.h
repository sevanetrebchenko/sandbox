
#ifndef SANDBOX_COMPONENT_H
#define SANDBOX_COMPONENT_H

namespace Sandbox {

    // All components need to derive from IComponent.
    struct IComponent {
        virtual ~IComponent() = 0;
    };

}

#endif //SANDBOX_COMPONENT_H
