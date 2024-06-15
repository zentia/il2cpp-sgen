#include "Include/Baselib.h"
#include "Source/Darwin/Baselib_Timer_DarwinApi.inl.h"

#include "Source/C99/Baselib_Memory_C99Api.inl.h"
#include "Source/Posix/Baselib_Memory_PosixApi.inl.h"
#include "Source/Darwin/Baselib_Memory_DarwinApi.inl.h"

namespace platform
{
    using DarwinApi::Baselib_Timer_GetTimeSinceStartupInSeconds;

    // Baselib_Memory
    using PosixApi::Baselib_Memory_GetPageSizeInfo;
    using C99Api::Baselib_Memory_Allocate;
    using C99Api::Baselib_Memory_Reallocate;
    using C99Api::Baselib_Memory_Free;
    using PosixApi::Baselib_Memory_AlignedAllocate;
    using PosixApi::Baselib_Memory_AlignedReallocate;
    using PosixApi::Baselib_Memory_AlignedFree;
    using DarwinApi::Baselib_Memory_AllocatePages;
    using PosixApi::Baselib_Memory_ReleasePages;
    using DarwinApi::Baselib_Memory_SetPageState;
}

#include "Source/Baselib_Config_Common_DarwinBased.inl.h"
