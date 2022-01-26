
#ifndef SANDBOX_HELPER_H
#define SANDBOX_HELPER_H

#include "pch.h"

#define PARAMETER_PACK_EXPAND(function, args, ...) (function<args>(__VA_ARGS__), ...)

#endif //SANDBOX_HELPER_H
