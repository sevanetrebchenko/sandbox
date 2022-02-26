
#ifndef SANDBOX_TIME_H
#define SANDBOX_TIME_H

#include "pch.h"
#include "common/utility/singleton.h"

namespace Sandbox {

    class Time : public Singleton<Time> {
        public:
            REGISTER_SINGLETON(Time);

            // Public fields.
            float dt;

        private:
            Time();
            ~Time() override;
    };

}

#endif //SANDBOX_TIME_H
