#include "il2cpp-config.h"
#include "il2cpp-api.h"
#include "vm/Runtime.h"
#include "gc/GarbageCollector.h"
#include "utils/Environment.h"

void il2cpp_init()
{
    tiny::vm::Runtime::Init();
}

void il2cpp_shutdown()
{
    tiny::vm::Runtime::Shutdown();
}

void il2cpp_gc_disable()
{
    il2cpp::gc::GarbageCollector::Disable();
}

void il2cpp_set_commandline_arguments(int argc, const char* const argv[], const char* basedir)
{
    il2cpp::utils::Environment::SetMainArgs(argv, argc);
}

void il2cpp_set_commandline_arguments_utf16(int argc, const Il2CppChar* const argv[], const char* basedir)
{
    il2cpp::utils::Environment::SetMainArgs(argv, argc);
}

void il2cpp_gc_wbarrier_set_field(Il2CppObject* obj, void** targetAddress, void* object)
{
    *targetAddress = object;
    il2cpp::gc::GarbageCollector::SetWriteBarrier(targetAddress);
}
