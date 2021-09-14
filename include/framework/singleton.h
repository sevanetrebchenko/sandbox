
#ifndef SANDBOX_SINGLETON_H
#define SANDBOX_SINGLETON_H

namespace Sandbox {

    template <typename Base>
    class Singleton {
        public:
            static Base* GetInstance();

            virtual void Initialize() = 0;
            virtual void Shutdown() = 0;

            // Singleton should not be copied/duplicated.
            Singleton(Singleton const &other) = delete;
            Singleton(Singleton &&other) = delete;
            void operator=(Singleton const &other) = delete;

        protected:
            Singleton();
            virtual ~Singleton() = default;

            bool initialized_;
    };

}

#include <framework/singleton.tpp>

// Must be included with every singleton instance.
#define REGISTER_SINGLETON(Base) friend class ::Sandbox::Singleton<Base>;

#endif //SANDBOX_SINGLETON_H
