
#ifndef SANDBOX_COMPONENT_SYSTEM_H
#define SANDBOX_COMPONENT_SYSTEM_H

namespace Sandbox {

    // Interface for making systems polymorphic at runtime.
    class ISystem {
        public:
            virtual void Init();
            virtual void Update() = 0;
            virtual void Shutdown();
    };


    template <typename T>
    class BaseSystem : public ISystem {
        public:
            BaseSystem();
            ~BaseSystem();

        protected:
            [[nodiscard]] T* GetComponent(int entityID) const;

            std::vector<int> entityIDs_; // IDs of entities updated by this system.
    };

}

#endif //SANDBOX_COMPONENT_SYSTEM_H
