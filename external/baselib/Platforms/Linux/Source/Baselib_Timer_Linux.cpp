#include "Include/Baselib.h"
#include "Source/Posix/Baselib_Timer_PosixApi.inl.h"
#include "Source/Cpp11/Baselib_Timer_Cpp11.inl.h"
#include "Source/Common/Baselib_Time_Common.inl.h"

namespace platform
{
    using PosixApi::Baselib_Timer_GetTicksToNanosecondsConversionRatio;
    using Cpp11Api::Baselib_Timer_WaitForAtLeast;
    BASELIB_INLINE_IMPL Baselib_Timer_Ticks Baselib_Timer_GetHighPrecisionTimerTicks()
    {
        return PosixApi::Baselib_Timer_GetHighPrecisionTimerTicks(CLOCK_MONOTONIC_RAW);
    }

    using Common::Baselib_Timer_GetTimeSinceStartupInSeconds;
}

#include "Source/CProxy/Baselib_Timer_CProxy.inl.h"
