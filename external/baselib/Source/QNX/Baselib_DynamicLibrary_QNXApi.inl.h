#pragma once

#include "Include/Baselib.h"
#include "Include/Cpp/Lock.h"
#include "Source/Baselib_Utilities.h"
#include "Source/LockRAII.h"
#include "Source/Posix/Baselib_DynamicLibrary_PosixApi.inl.h"

#include <algorithm>

namespace QNXApi
{
namespace detail
{
    // Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
    BASELIB_CPP_INTERFACE
    {
        struct DynamicLibrary
        {
            size_t refCount = 0;
            size_t canonicalPathHash = 0;
            void* libraryHandle = nullptr;

            void Reset()
            {
                memset(this, 0, sizeof(DynamicLibrary));
            }
        };

        class DynamicLibrariesContext
        {
        public:
            static DynamicLibrariesContext& Get()
            {
                static DynamicLibrariesContext context;
                return context;
            }

            DynamicLibrariesContext()
            {
                memset(libraries, 0, maxLibraries * sizeof(DynamicLibrary));
            }

            bool TryGetLibraryHandle(
                const char* pathnameUtf8,
                Baselib_DynamicLibrary_Handle& handle,
                Baselib_ErrorState& errorState
            )
            {
                BaselibAssert(errorState.code == Baselib_ErrorCode_Success);

                baselib::LockRAII lockScope(lock);

                const auto pathHash = GetCanonicalPathHashOrDefaultFrom(pathnameUtf8);
                if (size_t() == pathHash)
                {
                    errorState |= RaiseError(Baselib_ErrorCode_FailedToOpenDynamicLibrary);
                    return false;
                }

                auto slotIdx = FindLibraryOrNextFreeSlotIndex(pathHash);
                if (slotIdx < 0)
                {
                    errorState |= RaiseError(Baselib_ErrorCode_OutOfSystemResources)
                        | WithFormattedString("Hit a hard limit of at max %d loaded libraries", (int)maxLibraries);
                    return false;
                }

                auto& library = libraries[slotIdx];
                if (library.refCount == 0)
                {
                    handle = PosixApi::Baselib_DynamicLibrary_OpenUtf8(pathnameUtf8, &errorState);
                    if (errorState.code == Baselib_ErrorCode_Success)
                    {
                        library.refCount = 1;
                        library.canonicalPathHash = pathHash;
                        library.libraryHandle = (void*)handle.handle;
                        return true;
                    }
                    return false;
                }

                ++library.refCount;
                handle = ::detail::AsBaselibHandle<Baselib_DynamicLibrary_Handle>(library.libraryHandle);
                return true;
            }

            void CloseLibrary(Baselib_DynamicLibrary_Handle& handle)
            {
                baselib::LockRAII lockScope(lock);

                const auto endIt = std::end(libraries);
                auto it = std::find_if(std::begin(libraries), endIt,
                    [&handle](const DynamicLibrary& lib)
                    {
                        return ((void*)handle.handle) == lib.libraryHandle;
                    });
                if (it != endIt)
                {
                    auto& library = *it;
                    if (--library.refCount == 0)
                    {
                        auto handle = ::detail::AsBaselibHandle<Baselib_DynamicLibrary_Handle>(library.libraryHandle);
                        PosixApi::Baselib_DynamicLibrary_Close(handle);
                        library.Reset();
                    }
                }
            }

        private:
            int FindLibraryOrNextFreeSlotIndex(const size_t pathHash) const
            {
                int resultIdx = -1;

                for (size_t index = 0; index < maxLibraries; ++index)
                {
                    auto& library = libraries[index];
                    if (library.canonicalPathHash == pathHash)
                    {
                        resultIdx = (int)index;
                        break;
                    }
                    if (library.refCount == 0 && resultIdx == -1)
                    {
                        resultIdx = (int)index;
                    }
                }
                return resultIdx;
            }

            size_t GetCanonicalPathHashOrDefaultFrom(const char* path) const
            {
                const auto canonicalPath = GetCanonicalPathOrEmptyFrom(path);
                return canonicalPath.empty() ?
                    size_t() :
                    std::hash<std::string>()(canonicalPath);
            }

            std::string GetCanonicalPathOrEmptyFrom(const char* path) const
            {
                char buf[PATH_MAX] = {0};
                return realpath(path, buf) == NULL ? "" : buf;
            }

            baselib::Lock lock;

            // While there is no known documented limit for the number of shared objects,
            // the default value for the total number of file descriptors is 1000 which seems
            // to be a sensibly high value to reuse for the number of max libraries:
            // http://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.user_guide/topic/limits_FileDescriptors.html
            static const size_t maxLibraries = 1000;
            DynamicLibrary libraries[maxLibraries];
        };
    }
}

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf8(
        const char* pathnameUtf8, // can be nullptr
        Baselib_ErrorState* errorState
    )
    {
        auto& context = detail::DynamicLibrariesContext::Get();
        Baselib_DynamicLibrary_Handle handle;

        return context.TryGetLibraryHandle(pathnameUtf8, handle, *errorState) ?
            handle :
            Baselib_DynamicLibrary_Handle_Invalid;
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf16(
        const baselib_char16_t* pathnameUtf16,
        Baselib_ErrorState* errorState
    )
    {
        const auto pathnameUtf8 = ::detail::StrUtf16ToUtf8(pathnameUtf16);
        return QNXApi::Baselib_DynamicLibrary_OpenUtf8(pathnameUtf8.c_str(), errorState);
    }

    BASELIB_INLINE_IMPL void Baselib_DynamicLibrary_Close(
        Baselib_DynamicLibrary_Handle handle
    )
    {
        auto& context = detail::DynamicLibrariesContext::Get();

        context.CloseLibrary(handle);
    }
}
