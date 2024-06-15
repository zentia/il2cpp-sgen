#include "Source/Baselib_InlineImplementation.h"
#include "Source/Posix/Baselib_Memory_PosixApi.inl.h"
#include <malloc.h>

// Implementation of aligned allocators based on memalign which is deprecated according to https://linux.die.net/man/3/memalign
//
// Don't use Posix api for aligned alloc on Android since it got only fixed in NDK r17 (June 2018)
// See https://github.com/android/ndk/issues/647
// An alternative would be the C11 aligned_alloc, but it's available in C++ only with C++17 enabled.
//
// This is technically a Linux api and as such should go to Source/Linux,
// but some toolchains we use don't define it which would require extra #if to opt-in.
// As this is a deprecated Api we should use it only when necessary, so making Android only seems reasonable.

namespace Android
{
    BASELIB_INLINE_IMPL void* Baselib_Memory_AlignedAllocate(size_t size, size_t alignment)
    {
        void* allocatedMemory = ::memalign(alignment, size);
        if (OPTIMIZER_LIKELY(allocatedMemory != nullptr))
            return allocatedMemory;
        if (errno == ENOMEM)
            Baselib_Process_Abort(Baselib_ErrorCode_OutOfMemory);
        if (size == 0)
            return Baselib_Memory_AlignedAllocate(1, alignment);

        Baselib_Process_Abort(Baselib_ErrorCode_UnexpectedError);
    }

    // Alias the other aligned allocation from posix to clarify that the expectation is that
    // they are compatible with Android::Baselib_Memory_AlignedAllocate
    using PosixApi::Baselib_Memory_AlignedReallocate;
    using PosixApi::Baselib_Memory_AlignedFree;
}
