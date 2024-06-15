#include "Source/Baselib_Config_Common_LinuxBased.inl.h"
#include "Baselib_Memory_Android.inl.h"

#include <signal.h>
static_assert(SIGTRAP == DETAIL_BASELIB_SIGTRAP, "Baselib is not using the correct value for SIGTRAP");

namespace platform
{
    // Baselib_Memory
    using PosixApi::Baselib_Memory_GetPageSizeInfo;
    using C99Api::Baselib_Memory_Allocate;
    using C99Api::Baselib_Memory_Reallocate;
    using C99Api::Baselib_Memory_Free;
    using Android::Baselib_Memory_AlignedAllocate;
    using Android::Baselib_Memory_AlignedReallocate;
    using Android::Baselib_Memory_AlignedFree;
    using PosixApi::Baselib_Memory_AllocatePages;
    using PosixApi::Baselib_Memory_ReleasePages;
    using LinuxApi::Baselib_Memory_SetPageState;
}

#include "Source/CProxy/Baselib_Memory_CProxy.inl.h"
