#include "Source/Baselib_Config_Common_LinuxBased.inl.h"

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
}

#include "Source/CProxy/Baselib_Memory_CProxy.inl.h"
