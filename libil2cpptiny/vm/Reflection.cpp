#include "Reflection.h"
#include "gc/AppendOnlyGCHashMap.h"
#include "gc/GarbageCollector.h"
#include "os/ReaderWriterLock.h"
#include "utils/HashUtils.h"
#include "codegen/il2cpp-codegen.h"
#include "il2cpp-object-internals.h"

extern TinyType* g_SystemTypeTinyType;

typedef il2cpp::gc::AppendOnlyGCHashMap<intptr_t, Il2CppReflectionType*, il2cpp::utils::PassThroughHash<intptr_t> > TypeMap;
static TypeMap* s_TypeMap;

namespace tiny
{
namespace vm
{
    Il2CppReflectionType* Reflection::GetTypeObject(intptr_t handle)
    {
        Il2CppReflectionType* object = NULL;
        if (s_TypeMap->TryGetValue(handle, &object))
            return object;

        const size_t size = sizeof(Il2CppReflectionType);
        Il2CppReflectionType* typeObject = static_cast<Il2CppReflectionType*>(il2cpp::gc::GarbageCollector::Allocate(size));
        typeObject->typeHandle = reinterpret_cast<TinyType*>(handle);
        typeObject->object.klass = g_SystemTypeTinyType;

        return s_TypeMap->GetOrAdd(handle, typeObject);
    }

    void Reflection::Initialize()
    {
        s_TypeMap = new TypeMap();
    }
} /* namespace vm */
} /* namespace tiny */
