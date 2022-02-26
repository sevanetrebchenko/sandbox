
namespace Sandbox {

    template <typename Base>
    Base& Singleton<Base>::Instance() {
        static Base instance; // Scott Meyers singleton.
        return instance;
    }

    template <typename Base>
    Singleton<Base>::Singleton() = default;

    template <typename Base>
    Singleton<Base>::~Singleton() = default;

}