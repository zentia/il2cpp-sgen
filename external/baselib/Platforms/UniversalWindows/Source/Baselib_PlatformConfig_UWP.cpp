#include "Include/Baselib.h"
#include "Source/WinApi/Baselib_SystemFutex_WinApi.inl.h"
#include "Source/WinApi/Baselib_RegisteredNetwork_WinApi.inl.h"
#include "Source/WinApi/Baselib_Thread_WinApi.inl.h"
#include "Source/WinApi/Baselib_ThreadLocalStorage_FLS_WinApi.inl.h"

namespace platform
{
    // Baselib_RegisteredNetwork
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_Impl;
    using WinApi::Baselib_RegisteredNetwork_Buffer_Register;
    using WinApi::Baselib_RegisteredNetwork_Buffer_Deregister;
    using WinApi::Baselib_RegisteredNetwork_Endpoint_Create;
    using WinApi::Baselib_RegisteredNetwork_Endpoint_GetNetworkAddress;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_Create;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_ScheduleRecv;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_ScheduleSend;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_ProcessSend;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_ProcessRecv;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedSend;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedRecv;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_DequeueRecv;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_DequeueSend;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_GetNetworkAddress;
    using WinApi::Baselib_RegisteredNetwork_Socket_UDP_Close;

    // Baselib_SystemFutex
    using WinApi::Baselib_SystemFutex_Wait;
    using WinApi::Baselib_SystemFutex_Notify;

    // Baselib_Thread
    constexpr auto Baselib_Thread_Create = WinApi::Baselib_Thread_Create<WinApi::SetThreadName_ViaException>;
    using WinApi::Baselib_Thread_Join;
    using WinApi::Baselib_Thread_YieldExecution;
    using WinApi::Baselib_Thread_GetCurrentThreadId;
    BASELIB_INLINE_IMPL bool Baselib_Thread_SupportsThreads() { return true; }

    // Baselib_TLS
    using WinApi::FLS::Baselib_TLS_Alloc;
    using WinApi::FLS::Baselib_TLS_Free;
}

#include "Source/CProxy/Baselib_RegisteredNetwork_CProxy.inl.h"
#include "Source/CProxy/Baselib_SystemFutex_CProxy.inl.h"
#include "Source/CProxy/Baselib_Thread_CProxy.inl.h"
#include "Source/CProxy/Baselib_ThreadLocalStorage_CProxy.inl.h"
#include "Source/Baselib_Config_Common_WinApiBased.inl.h"

// Baselib_Timer_GetTicksToNanosecondsConversionRatio:
// Measured: 1000000000 / 1745597013 = 0.572869908
// (2018/10/08, Hostmachine: Win64 10.0.17134, Bee SDK: Xdk_180501, Xbox OS version: 10.0.17134.4071 (June 2018 QFE 1), Device ID: FD00DF259150ED4F)
