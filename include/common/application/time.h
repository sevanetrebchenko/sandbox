
#ifndef SANDBOX_TIME_H
#define SANDBOX_TIME_H

#include "pch.h"

namespace Sandbox {

    class Time {
        public:
            static Time& Instance();
            float dt;

        private:
            Time();
            ~Time();
    };

}

#endif //SANDBOX_TIME_H
