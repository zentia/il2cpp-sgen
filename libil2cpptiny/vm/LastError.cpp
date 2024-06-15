#include "LastError.h"
#include "os/LastError.h"

namespace tiny
{
namespace vm
{
    uint32_t LastError::s_LastError = 0;

    uint32_t LastError::GetLastError()
    {
        return s_LastError;
    }

    void LastError::StoreLastError()
    {
        // Get the last error first, before any other calls (so that we don't stomp on it).
        uint32_t lastError = il2cpp::os::LastError::GetLastError();

        s_LastError = lastError;
    }
} /* namespace vm */
} /* namespace tiny */
