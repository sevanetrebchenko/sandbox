
#ifndef SANDBOX_SINGLETON_TPP
#define SANDBOX_SINGLETON_TPP

namespace Sandbox {

    template <typename Base>
    Base* Singleton<Base>::GetInstance() {
        static Base instance; // Scott Meyers singleton.
        return &instance;
    }

    template <typename Base>
    Singleton<Base>::Singleton() : initialized_(false) {
    }

}

#endif //SANDBOX_SINGLETON_TPP