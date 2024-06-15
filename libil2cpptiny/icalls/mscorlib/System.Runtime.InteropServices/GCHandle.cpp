#include "il2cpp-config.h"
#include "il2cpp-class-internals.h"
#include "GCHandle.h"
#include "gc/GCHandle.h"
#include "vm/Object.h"
#include "vm/Exception.h"
#include "vm/Type.h"

extern TinyType* g_StringTinyType;
extern TinyType* g_ArrayTinyType;

namespace tiny
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
namespace Runtime
{
namespace InteropServices
{
    void GCHandle::FreeHandle(int32_t handle)
    {
        il2cpp::gc::GCHandle::Free(handle);
    }

    Il2CppObject* GCHandle::GetTarget(int32_t handle)
    {
        return il2cpp::gc::GCHandle::GetTarget(handle);
    }

    int32_t GCHandle::GetTargetHandle(Il2CppObject* obj, int32_t handle, int32_t type)
    {
        return il2cpp::gc::GCHandle::GetTargetHandle(obj, handle, type).Get();
    }

    static bool IsArray(TinyType* type)
    {
        uint8_t typeHierarchySize = type->typeHierarchySize;
        if (typeHierarchySize == 0)
            return false;

        Il2CppReflectionType* baseType = vm::Type::GetTypeFromHandle((intptr_t)(type->GetTypeHierarchy()[typeHierarchySize - 1]));
        return g_ArrayTinyType != NULL && baseType->typeHandle == g_ArrayTinyType;
    }

    intptr_t GCHandle::GetAddrOfPinnedObject(int32_t handle)
    {
        il2cpp::gc::GCHandleType type = il2cpp::gc::GCHandle::GetHandleType(handle);

        if (type != il2cpp::gc::HANDLE_PINNED)
            return reinterpret_cast<intptr_t>(reinterpret_cast<uint8_t*>(-2)); // mscorlib on managed land expects us to return "-2" as IntPtr if this condition occurs

        Il2CppObject* obj = il2cpp::gc::GCHandle::GetTarget(handle);
        if (obj == NULL)
            return 0;

        ptrdiff_t offset;

        if (IsArray(obj->klass))
        {
            // Pointer to first array element
            offset = kIl2CppSizeOfArray;
        }
        else if (obj->klass == g_StringTinyType)
        {
            // Pointer to first character
            offset = offsetof(Il2CppString, chars);
        }
        else
        {
            // Pointer to struct in boxed object
            offset = IL2CPP_ALIGNED_OBJECT_SIZE;
        }

        return reinterpret_cast<intptr_t>((reinterpret_cast<uint8_t*>(obj) + offset));
    }
} /* namespace InteropServices */
} /* namespace Runtime */
} /* namespace System */
} /* namespace mscorlib */
} /* namespace icalls */
} /* namespace tiny */
