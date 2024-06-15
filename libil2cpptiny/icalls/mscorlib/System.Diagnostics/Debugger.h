#pragma once

#include <stdint.h>
#include "il2cpp-config.h"
#include "il2cpp-object-internals.h"

namespace tiny
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
namespace Diagnostics
{
    class LIBIL2CPP_CODEGEN_API Debugger
    {
    public:
        static bool IsAttached_internal();

        static bool IsLogging();
        static void Log(int32_t level, Il2CppString* category, Il2CppString* message);
    };
} /* namespace Diagnostics */
} /* namespace System */
} /* namespace mscorlib */
} /* namespace icalls */
} /* namespace tiny */
