#include "Include/Baselib.h"
#include "Source/Posix/Baselib_Timer_PosixApi.inl.h"
#include "Source/Cpp11/Baselib_Timer_Cpp11.inl.h"
#include "Source/Common/Baselib_Time_Common.inl.h"
#include "Source/Baselib_Config_Common_LinuxBased.inl.h"

#include <signal.h>
static_assert(SIGTRAP == DETAIL_BASELIB_SIGTRAP, "Baselib is not using the correct value for SIGTRAP");

namespace platform
{
    // Baselib_Memory
    using PosixApi::Baselib_Memory_GetPageSizeInfo;
    using C99Api::Baselib_Memory_Allocate;
    using C99Api::Baselib_Memory_Reallocate;
    using C99Api::Baselib_Memory_Free;
    using PosixApi::Baselib_Memory_AlignedAllocate;
    using PosixApi::Baselib_Memory_AlignedReallocate;
    using PosixApi::Baselib_Memory_AlignedFree;
    using PosixApi::Baselib_Memory_AllocatePages;
    using PosixApi::Baselib_Memory_ReleasePages;
    using LinuxApi::Baselib_Memory_SetPageState;

    // Timer
    using PosixApi::Baselib_Timer_GetTicksToNanosecondsConversionRatio;
    using Cpp11Api::Baselib_Timer_WaitForAtLeast;
    using Common::Baselib_Timer_GetTimeSinceStartupInSeconds;

    BASELIB_INLINE_IMPL Baselib_Timer_Ticks Baselib_Timer_GetHighPrecisionTimerTicks()
    {
        return PosixApi::Baselib_Timer_GetHighPrecisionTimerTicks(CLOCK_MONOTONIC_RAW);
    }
}

#include "Source/CProxy/Baselib_Timer_CProxy.inl.h"
#include "Source/CProxy/Baselib_Memory_CProxy.inl.h"
