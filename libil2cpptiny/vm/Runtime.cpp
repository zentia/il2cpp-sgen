#include "il2cpp-config.h"
#include "Runtime.h"
#include "TypeUniverse.h"
#include "os/CrashHelpers.h"
#include "os/Image.h"
#include "os/Memory.h"
#include "os/ThreadLocalValue.h"
#include "gc/GarbageCollector.h"
#include "utils/Logging.h"
#include "vm/DebugMetadata.h"
#include "vm/StackTrace.h"
#include "vm/Reflection.h"

void Il2CppCallStaticConstructors();
extern const void** GetStaticFieldsStorageArray();

namespace tiny
{
namespace vm
{
    // This is an abitrary value used to ensure we can
    // identify the main thread.
    const uintptr_t s_MainThreadTlsValue = 0x8811;
    static il2cpp::os::ThreadLocalValue s_MainThreadTls;

    void Runtime::Init()
    {
#if IL2CPP_ENABLE_STACKTRACES
        vm::StackTrace::InitializeStackTracesForCurrentThread();
#endif
        il2cpp::gc::GarbageCollector::Initialize();
        Reflection::Initialize();
        TypeUniverse::Initialize();
        AllocateStaticFieldsStorage();
        Il2CppCallStaticConstructors();
#if IL2CPP_ENABLE_STACKTRACES
        il2cpp::os::Image::Initialize();
        vm::DebugMetadata::InitializeMethodsForStackTraces();
#endif
        s_MainThreadTls.SetValue(reinterpret_cast<void*>(s_MainThreadTlsValue));
    }

    void Runtime::Shutdown()
    {
        FreeStaticFieldsStorage();
        il2cpp::gc::GarbageCollector::UninitializeGC();
#if IL2CPP_ENABLE_STACKTRACES
        vm::StackTrace::CleanupStackTracesForCurrentThread();
#endif
    }

    void Runtime::AllocateStaticFieldsStorage()
    {
        const void** StaticFieldsStorage = GetStaticFieldsStorageArray();
        int i = 0;
        while (StaticFieldsStorage[i] != NULL)
        {
            *(void**)StaticFieldsStorage[i] = il2cpp::gc::GarbageCollector::AllocateFixed(*(size_t*)StaticFieldsStorage[i], NULL);
            i++;
        }
    }

    void Runtime::FreeStaticFieldsStorage()
    {
        const void** StaticFieldsStorage = GetStaticFieldsStorageArray();
        int i = 0;
        while (StaticFieldsStorage[i] != NULL)
        {
            il2cpp::gc::GarbageCollector::FreeFixed(*(void**)StaticFieldsStorage[i]);
            i++;
        }
    }

    void Runtime::FailFast(const char* message)
    {
        bool messageWritten = false;
        if (message != NULL)
        {
            if (strlen(message) != 0)
            {
                il2cpp::utils::Logging::Write(message);
                messageWritten = true;
            }
        }

        if (!messageWritten)
            il2cpp::utils::Logging::Write("No error message was provided. Hopefully the stack trace can provide some information.");

        StackTrace::LogManagedStackTrace();

        il2cpp::os::CrashHelpers::Crash();
    }

    void Runtime::FailIfNotMainThread()
    {
        void* currentThreadTls = 0;
        s_MainThreadTls.GetValue(&currentThreadTls);
        if ((uintptr_t)currentThreadTls != s_MainThreadTlsValue)
            FailFast("Managed code must be compiled with Burst to execute on a non-main thread.");
    }
}
}
