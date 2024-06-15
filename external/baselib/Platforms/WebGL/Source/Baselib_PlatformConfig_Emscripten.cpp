#include "Include/Baselib.h"
#include "Source/Posix/Baselib_FileIO_PosixApi.inl.h"
#include "Source/Posix/Baselib_Memory_PosixApi.inl.h"
#include "Source/Posix/Baselib_Timer_PosixApi.inl.h"
#include "Source/Posix/Baselib_DynamicLibrary_PosixApi.inl.h"

#include "Source/C99/Baselib_Memory_C99Api.inl.h"
#include "Source/C99/Baselib_Process_C99Api.inl.h"

#include "Source/Common/Baselib_RegisteredNetwork_Common.inl.h"
#include "Source/Common/Baselib_FileIO_Common.inl.h"

#include "Baselib_Memory_Emscripten.inl.h"

#ifndef __EMSCRIPTEN_PTHREADS__
#include "Source/NoThreads/Baselib_SystemFutex_NoThreads.inl.h"
#include "Source/NoThreads/Baselib_SystemSemaphore_NoThreads.inl.h"
#include "Source/NoThreads/Baselib_Thread_NoThreads.inl.h"
#include "Source/NoThreads/Baselib_ThreadLocalStorage_NoThreads.inl.h"

namespace Emscripten
{
    using namespace NoThreads;
}
#else
#include "Baselib_SystemFutex_Emscripten.inl.h"
#include "Baselib_SystemSemaphore_Emscripten.inl.h"
#include "Baselib_Thread_Emscripten.inl.h"
#include "Baselib_ThreadLocalStorage_Emscripten.inl.h"

namespace Emscripten
{
    using namespace Emscripten_WithPThreads;
}
#endif

namespace platform
{
    // Baselib_Debug
    // Custom implementation, see Baselib_Debug_Emscripten.cpp

    // Baselib_Process
    using C99Api::Baselib_Process_Abort;

    // Baselib_SystemSemaphore
    using Emscripten::Baselib_SystemSemaphore_Create;
    using Emscripten::Baselib_SystemSemaphore_CreateInplace;
    using Emscripten::Baselib_SystemSemaphore_Acquire;
    using Emscripten::Baselib_SystemSemaphore_TryAcquire;
    using Emscripten::Baselib_SystemSemaphore_TryTimedAcquire;
    using Emscripten::Baselib_SystemSemaphore_Release;
    using Emscripten::Baselib_SystemSemaphore_Free;
    using Emscripten::Baselib_SystemSemaphore_FreeInplace;

    // Baselib_Memory
    // TODO: Newer SDK versions expose `aligned_realloc` directly. We should use this once available in Unity!
    // See https://github.com/emscripten-core/emscripten/blob/d4c3592/system/include/emscripten/emmalloc.h#L64
    using PosixApi::Baselib_Memory_GetPageSizeInfo;
    using C99Api::Baselib_Memory_Allocate;
    using C99Api::Baselib_Memory_Reallocate;
    using C99Api::Baselib_Memory_Free;
    using PosixApi::Baselib_Memory_AlignedAllocate;
    using PosixApi::Baselib_Memory_AlignedReallocate;
    using PosixApi::Baselib_Memory_AlignedFree;
    using Emscripten::Baselib_Memory_AllocatePages;
    using Emscripten::Baselib_Memory_SetPageState;
    using PosixApi::Baselib_Memory_ReleasePages;

    // Baselib_RegisteredNetwork
    using Common::Baselib_RegisteredNetwork_Socket_UDP_Impl;
    using Common::Baselib_RegisteredNetwork_Buffer_Register;
    using Common::Baselib_RegisteredNetwork_Buffer_Deregister;
    using Common::Baselib_RegisteredNetwork_Endpoint_Create;
    using Common::Baselib_RegisteredNetwork_Endpoint_GetNetworkAddress;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_Create;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ScheduleRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ScheduleSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ProcessSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ProcessRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_DequeueRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_DequeueSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_GetNetworkAddress;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_Close;

    // Baselib_Socket
    // Custom implementation, see Baselib_Socket_Emscripten.cpp

    // Baselib_SystemFutex
    using Emscripten::Baselib_SystemFutex_Wait;
    using Emscripten::Baselib_SystemFutex_Notify;

    // Baselib_TLS
    using Emscripten::Baselib_TLS_Alloc;
    using Emscripten::Baselib_TLS_Free;

    // Baselib_Timer
    // Custom implementation, see Baselib_Timer_Emscripten.cpp

    // Baselib_Thread
    using Emscripten::Baselib_Thread_Create;
    using Emscripten::Baselib_Thread_Join;
    using Emscripten::Baselib_Thread_YieldExecution;
    using Emscripten::Baselib_Thread_GetCurrentThreadId;
    using Emscripten::Baselib_Thread_SupportsThreads;

    // Baselib_FileIO
    using Common::Baselib_FileIO_EventQueue_Create;
    using Common::Baselib_FileIO_EventQueue_Free;
    using Common::Baselib_FileIO_EventQueue_Dequeue;
    using Common::Baselib_FileIO_EventQueue_Shutdown;
    using Common::Baselib_FileIO_AsyncOpen;
    using Common::Baselib_FileIO_AsyncRead;
    using Common::Baselib_FileIO_AsyncClose;
    using PosixApi::Baselib_FileIO_SyncOpen;
    using PosixApi::Baselib_FileIO_SyncFileFromNativeHandle;
    using PosixApi::Baselib_FileIO_SyncRead;
    using PosixApi::Baselib_FileIO_SyncWrite;
    using PosixApi::Baselib_FileIO_SyncFlush;
    using PosixApi::Baselib_FileIO_SyncSetFileSize;
    using PosixApi::Baselib_FileIO_SyncGetFileSize;
    using PosixApi::Baselib_FileIO_SyncClose;

    // Baselib_DynamicLibrary
    using PosixApi::Baselib_DynamicLibrary_OpenUtf8;
    using PosixApi::Baselib_DynamicLibrary_OpenUtf16;
    using PosixApi::Baselib_DynamicLibrary_OpenProgramHandle;
    using PosixApi::Baselib_DynamicLibrary_FromNativeHandle;
    using PosixApi::Baselib_DynamicLibrary_GetFunction;
    using PosixApi::Baselib_DynamicLibrary_Close;
}

#include "Source/CProxy/Baselib_Memory_CProxy.inl.h"
#include "Source/CProxy/Baselib_Process_CProxy.inl.h"
#include "Source/CProxy/Baselib_RegisteredNetwork_CProxy.inl.h"
#include "Source/CProxy/Baselib_SystemFutex_CProxy.inl.h"
#include "Source/CProxy/Baselib_SystemSemaphore_CProxy.inl.h"
#include "Source/CProxy/Baselib_Thread_CProxy.inl.h"
#include "Source/CProxy/Baselib_ThreadLocalStorage_CProxy.inl.h"
#include "Source/CProxy/Baselib_FileIO_CProxy.inl.h"
#include "Source/CProxy/Baselib_DynamicLibrary_CProxy.inl.h"
