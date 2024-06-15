#pragma once

#include "il2cpp-config.h"

#if IL2CPP_TARGET_ANDROID
	#include <android/log.h>
	#define IL2CPP_TRACE(...) __android_log_print(ANDROID_LOG_DEBUG, "IL2CPP", __VA_ARGS__)
#else
	#define IL2CPP_TRACE(...)
#endif

namespace il2cpp
{
namespace utils
{
    class LIBIL2CPP_CODEGEN_API Logging
    {
    public:
        static void Write(const char* format, ...);
        static void SetLogCallback(Il2CppLogCallback method);
        static bool IsLogCallbackSet();

    private:
        static Il2CppLogCallback s_Callback;
    };
}
}
