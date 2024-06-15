#include "il2cpp-config.h"

#include "Debugger.h"
#include "vm-utils/Debugger.h"

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
    bool Debugger::IsAttached_internal()
    {
        // The debugger with Tiny always uses big libil2cpp,
        // so libil2cpptiny will never have the debugger attached.
        return false;
    }
} /* namespace Diagnostics */
} /* namespace System */
} /* namespace mscorlib */
} /* namespace icalls */
} /* namespace tiny */
