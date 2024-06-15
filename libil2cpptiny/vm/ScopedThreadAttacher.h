#pragma once
#include "vm/Runtime.h"

namespace tiny
{
namespace vm
{
    struct ScopedThreadAttacher
    {
        ScopedThreadAttacher()
        {
            vm::Runtime::FailIfNotMainThread();
        }

        ~ScopedThreadAttacher()
        {
        }
    };
} /* namespace vm */
} /* namespace il2cpp */
