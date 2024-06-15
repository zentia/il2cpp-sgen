#pragma once

#include "il2cpp-config.h"
#include "il2cpp-object-internals.h"

#include "Object.h"
#include "gc/GarbageCollector.h"

namespace tiny
{
namespace vm
{
    class LIBIL2CPP_CODEGEN_API Array
    {
    public:
        template<typename T>
        static Il2CppArray* New(TinyType* arrayType, uint32_t elementSize, uint32_t arrayLength)
        {
            COMPILE_TIME_CONST size_t alignedArraySize = (sizeof(Il2CppArray) + ALIGN_OF(T) - 1) & ~(ALIGN_OF(T) - 1);
            Il2CppArray* szArray = static_cast<Il2CppArray*>(Object::New(alignedArraySize + elementSize * arrayLength, arrayType));
            szArray->max_length = arrayLength;
            return szArray;
        }
    };
} /* namespace vm */
} /* namespace tiny */

inline char* il2cpp_array_addr_with_size(Il2CppArray *array, int32_t size, uintptr_t idx)
{
    return ((char*)array) + kIl2CppSizeOfArray + size * idx;
}

#define il2cpp_array_addr(array, type, index) ((type*)(void*) il2cpp_array_addr_with_size (array, sizeof (type), index))
#define il2cpp_array_get(array, type, index) ( *(type*)il2cpp_array_addr ((array), type, (index)) )
#define il2cpp_array_setref(array, index, value)  \
    do {    \
        void* * __p = (void* *) il2cpp_array_addr ((array), void*, (index)); \
         *__p = (value);    \
        il2cpp::gc::GarbageCollector::SetWriteBarrier(__p); \
    } while (0)
#define il2cpp_array_setrefwithsize(array, elementSize, index, value)  \
    do {    \
        void* __p = (void*) il2cpp_array_addr_with_size ((array), elementSize, (index)); \
        memcpy(__p, value, elementSize); \
        il2cpp::gc::GarbageCollector::SetWriteBarrier((void**)__p); \
    } while (0)
