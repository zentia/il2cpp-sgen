#pragma once

#include "il2cpp-config.h"
#include "il2cpp-object-internals.h"

#include "gc/GarbageCollector.h"

namespace tiny
{
namespace vm
{
    class LIBIL2CPP_CODEGEN_API Object
    {
    public:
        static void* Unbox(Il2CppObject* obj)
        {
            return reinterpret_cast<uint8_t*>(obj) + IL2CPP_ALIGNED_OBJECT_SIZE;
        }

        static Il2CppObject* New(size_t size, TinyType* typeInfo)
        {
            Il2CppObject* result = static_cast<Il2CppObject*>(il2cpp::gc::GarbageCollector::AllocateObject(size, typeInfo));
            IL2CPP_ASSUME(result > NULL);
            result->klass = typeInfo;
            return result;
        }
    };
} /* namespace vm */
} /* namespace tiny */
