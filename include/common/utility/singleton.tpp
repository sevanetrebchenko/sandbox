
namespace Sandbox {

    template <typename Base>
    Base& ISingleton<Base>::Instance() {
        static Base instance; // Scott Meyers singleton.
        return instance;
    }

    template <typename Base>
    ISingleton<Base>::ISingleton() = default;

    template <typename Base>
    ISingleton<Base>::~ISingleton() = default;

}