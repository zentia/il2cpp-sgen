#include "Include/Baselib.h"
#include "Source/Posix/Baselib_Socket_PosixApi.inl.h"

namespace platform
{
#ifdef BASELIB_SOCKET_EMSCRIPTEN_SUPPORTED
    // Sockets can be fully supported in emscripten when a special proxy is also running which translates each
    // API call in parity with posix as special messages over a websockets connection. While this allows
    // interesting web support for features such as TCP connections, it is very slow as well as may pose
    // security risks in production software. It is currently used primarily to support connection to a
    // managed debugger from locally hosted web builds during the development phase of DOTS Runtime based
    // web projects.
    using PosixApi::Baselib_Socket_Create;
#else
    Baselib_Socket_Handle Baselib_Socket_Create(Baselib_NetworkAddress_Family family, Baselib_Socket_Protocol protocol, Baselib_ErrorState* errorState)
    {
        errorState |= Validate(family);
        errorState |= Validate(protocol);

        // This essentially disables sockets for emscripten.
        // We need to figure out what our options are. Posix usage compiles but has a _very_ limited feature set.
        errorState |= RaiseError(Baselib_ErrorCode_AddressFamilyNotSupported);
        return Baselib_Socket_Handle_Invalid;
    }

#endif

    using PosixApi::Baselib_Socket_Bind;
    using PosixApi::Baselib_Socket_TCP_Connect;
    using PosixApi::Baselib_Socket_Poll;
    using PosixApi::Baselib_Socket_GetAddress;
    using PosixApi::Baselib_Socket_TCP_Listen;
    using PosixApi::Baselib_Socket_TCP_Accept;
    using PosixApi::Baselib_Socket_UDP_Send;
    using PosixApi::Baselib_Socket_TCP_Send;
    using PosixApi::Baselib_Socket_UDP_Recv;
    using PosixApi::Baselib_Socket_TCP_Recv;
    using PosixApi::Baselib_Socket_Close;
}

#include "Source/CProxy/Baselib_Socket_CProxy.inl.h"
