#pragma once

#include "il2cpp-config-api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

void il2cpp_init();
void il2cpp_shutdown();
void il2cpp_gc_disable();
void il2cpp_set_commandline_arguments(int argc, const char* const argv[], const char* basedir);
void il2cpp_set_commandline_arguments_utf16(int argc, const Il2CppChar* const argv[], const char* basedir);

void il2cpp_gc_wbarrier_set_field(Il2CppObject* obj, void** targetAddress, void* object);

#if defined(__cplusplus)
}
#endif
