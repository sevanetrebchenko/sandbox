
#include <framework/time.h>

namespace Sandbox {

    Time& Time::Instance() {
        static Time instance;
        return instance;
    }

    Time::Time() : dt(0) {
    }

    Time::~Time() {
    }

}


