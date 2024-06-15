#include "Include/Baselib.h"
#include "Source/Cpp11/Baselib_Timer_Cpp11.inl.h"
#include "Source/Posix/Baselib_Timer_PosixApi.inl.h"
#include "Source/ClockCombiner.h"

namespace detail
{
    // Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
    BASELIB_CPP_INTERFACE
    {
        static double GetTimeAsSeconds(int clockClass)
        {
            timespec time;
            int clockError = clock_gettime(clockClass, &time);
            BaselibAssert(!clockError, "Could not get time.");
            const double secondsPerNanosecond = 1.0 / Baselib_NanosecondsPerSecond;
            return time.tv_sec + (double)time.tv_nsec * secondsPerNanosecond;
        }

        // This construct is here to ensure that Baselib_Timer_GetTimeSinceStartupInSeconds is called during static initialization
        static struct AndroidStartupTimeInitializer
        {
            AndroidStartupTimeInitializer()
            {
                Baselib_Timer_GetTimeSinceStartupInSeconds();
            }
        } androidStartupTimeInitializer;
    }
}

#define DONT_STRIP(address) UNUSED(address)

namespace platform
{
    using PosixApi::Baselib_Timer_GetTicksToNanosecondsConversionRatio;
    using Cpp11Api::Baselib_Timer_WaitForAtLeast;
    BASELIB_INLINE_IMPL Baselib_Timer_Ticks Baselib_Timer_GetHighPrecisionTimerTicks()
    {
        return PosixApi::Baselib_Timer_GetHighPrecisionTimerTicks(CLOCK_MONOTONIC);
    }

    BASELIB_INLINE_IMPL double Baselib_Timer_GetTimeSinceStartupInSeconds()
    {
        DONT_STRIP(detail::androidStartupTimeInitializer);
        // The reason for having this as a scoped static and not a file static is to avoid the ordering
        // problems of file static initialization.
        static TimeSinceStartupMonotonicBoottimeClockCombiner s_TimeSinceStartupTracker;
        return s_TimeSinceStartupTracker.GetTimeSinceStartup(
            detail::GetTimeAsSeconds(CLOCK_MONOTONIC), detail::GetTimeAsSeconds(CLOCK_BOOTTIME));
    }
}

#include "Source/CProxy/Baselib_Timer_CProxy.inl.h"
