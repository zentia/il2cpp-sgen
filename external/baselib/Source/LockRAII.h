#pragma once

#include "Include/Cpp/Lock.h"

namespace baselib
{
    BASELIB_CPP_INTERFACE
    {
        // Provides an object that acquires a given lock on creation
        // and releases the lock on object destruction.
        struct LockRAII
        {
            LockRAII(baselib::Lock& setLock)
                : lock(setLock)
            {
                lock.Acquire();
            }

            ~LockRAII()
            {
                lock.Release();
            }

            baselib::Lock& lock;
        };
    }
}
