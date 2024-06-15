#pragma once

#include <stdint.h>
#include "il2cpp-config.h"

namespace tiny
{
namespace vm
{
    class LIBIL2CPP_CODEGEN_API LastError
    {
    public:
        static uint32_t GetLastError();
        static void StoreLastError();

    private:
        static uint32_t s_LastError;
    };
} /* namespace vm */
} /* namespace tiny */
