
#pragma once

namespace Sandbox {

    template <typename Base>
    class Singleton {
        public:
            static Base& Instance();

            // Singleton should not be copied/duplicated.
            Singleton(Singleton const &other) = delete;
            Singleton(Singleton &&other) = delete;
            void operator=(Singleton const &other) = delete;

        protected:
            Singleton();
            virtual ~Singleton();
    };

#define REGISTER_SINGLETON(TYPE) friend class ::Sandbox::Singleton<TYPE>;

}

#include "common/utility/singleton.tpp"