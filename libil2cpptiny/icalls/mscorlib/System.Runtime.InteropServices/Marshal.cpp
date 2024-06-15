#include "il2cpp-config.h"

#include "Marshal.h"
#include "vm/LastError.h"

namespace tiny
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
namespace Runtime
{
namespace InteropServices
{
    int32_t Marshal::GetLastWin32Error()
    {
        return vm::LastError::GetLastError();
    }
} /* namespace InteropServices */
} /* namespace Runtime */
} /* namespace System */
} /* namespace mscorlib */
} /* namespace icalls */
} /* namespace tiny */
