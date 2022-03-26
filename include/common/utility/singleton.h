
#pragma once

namespace Sandbox {

    template <typename Base>
    class ISingleton {
        public:
            static Base& Instance();

            // Singleton should not be copied/duplicated.
            ISingleton(ISingleton const &other) = delete;
            ISingleton(ISingleton &&other) = delete;
            void operator=(ISingleton const &other) = delete;

        protected:
            ISingleton();
            virtual ~ISingleton();
    };

#define REGISTER_SINGLETON(TYPE) friend class ::Sandbox::ISingleton<TYPE>;

}

#include "common/utility/singleton.tpp"