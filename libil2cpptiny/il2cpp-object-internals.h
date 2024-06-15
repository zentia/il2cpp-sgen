#pragma once

#if IL2CPP_TINY

#include "il2cpp-class-internals.h"
#include "il2cpp-runtime-metadata.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef struct TinyType TinyType;

typedef struct Il2CppObject
{
    TinyType* klass;
} Il2CppObject;

#ifdef __cplusplus
typedef struct Il2CppArray : public Il2CppObject
{
#else
typedef struct Il2CppArray
{
    Il2CppObject obj;
#endif
    il2cpp_array_size_t max_length;
} Il2CppArray;

#ifdef __cplusplus
template<size_t N>
struct Il2CppMultidimensionalArray : public Il2CppObject
{
    il2cpp_array_size_t bounds[N];
};
#endif

#if IL2CPP_COMPILER_MSVC
#pragma warning( push )
#pragma warning( disable : 4200 )
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

#ifdef __cplusplus
typedef struct Il2CppArraySize : public Il2CppArray
{
#else
typedef struct Il2CppArraySize
{
    Il2CppObject obj;
    Il2CppArrayBounds *bounds;
    il2cpp_array_size_t max_length;
#endif //__cplusplus
    ALIGN_TYPE(8) void* vector[IL2CPP_ZERO_LEN_ARRAY];
} Il2CppArraySize;

static const size_t kIl2CppSizeOfArray = (offsetof(Il2CppArraySize, vector));

typedef struct Il2CppString
{
    Il2CppObject object;
    int32_t length;
    Il2CppChar chars[IL2CPP_ZERO_LEN_ARRAY];
} Il2CppString;

#if IL2CPP_COMPILER_MSVC
#pragma warning( pop )
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef __cplusplus
typedef struct Il2CppDelegate : Il2CppObject
{
#else
typedef struct Il2CppDelegate
{
    Il2CppObject obj;
#endif
    void* method_ptr;
    Il2CppObject* m_target;
    void* invoke_impl;
    void* multicast_invoke_impl;
    void* m_ReversePInvokeWrapperPtr;
    bool m_IsDelegateOpen;
} Il2CppDelegate;

#ifdef __cplusplus
typedef struct Il2CppMulticastDelegate : Il2CppDelegate
{
#else
typedef struct Il2CppMulticastDelegate
{
    Il2CppDelegate delegate;
#endif
    Il2CppArray* delegates;
    int delegateCount;
} Il2CppMulticastDelegate;

typedef struct Il2CppReflectionType
{
    Il2CppObject object;
    const TinyType* typeHandle;
} Il2CppReflectionType;

#ifdef __cplusplus
// System.Exception
typedef struct Il2CppException : public Il2CppObject
{
#else
typedef struct Il2CppException
{
    Il2CppObject object;
#endif //__cplusplus
    Il2CppString* message;
    Il2CppString* stack_trace;
} Il2CppException;

typedef struct Il2CppExceptionWrapper
{
    Il2CppException* ex;
#ifdef __cplusplus
    Il2CppExceptionWrapper(Il2CppException* ex) : ex(ex) {}
#endif //__cplusplus
} Il2CppExceptionWrapper;

// Interface offsets are 16 bits wide, so we pack two into each entry in the tiny type universe
// for both 32-bit builds and four entries for 64-bit builds.
#if IL2CPP_SIZEOF_VOID_P == 8
const int NumberOfPackedInterfaceOffsetsPerElement = 4;
#else
const int NumberOfPackedInterfaceOffsetsPerElement = 2;
#endif

#define IL2CPP_TINY_VTABLE_SIZE(packedVtableSizeAndAdditionalTypeMetadata) (packedVtableSizeAndAdditionalTypeMetadata & 0x1FFF)
#define IL2CPP_TINY_ADDITIONAL_TYPE_METADATA(packedVtableSizeAndAdditionalTypeMetadata) ((packedVtableSizeAndAdditionalTypeMetadata & 0xE000) >> 13)

typedef uint16_t packed_iterface_offset_t;

#ifdef __cplusplus
typedef struct TinyType : Il2CppObject
{
#else
typedef struct TinyType
{
    Il2CppObject obj;
#endif
    // The three high bits are used for additional type metadata.
    // The lower 13 bits are used for the vtable count. We support
    // 8192 (2^13) virtual methods.
    uint16_t packedVtableSizeAndAdditionalTypeMetadata;
    uint8_t typeHierarchySize;
    uint8_t interfacesSize;
#ifdef __cplusplus
    inline const Il2CppMethodPointer* GetVTable() const
    {
        return reinterpret_cast<const Il2CppMethodPointer*>(this + 1);
    }

    inline const TinyType* const* GetTypeHierarchy() const
    {
        return reinterpret_cast<const TinyType* const*>(GetVTable() + IL2CPP_TINY_VTABLE_SIZE(packedVtableSizeAndAdditionalTypeMetadata));
    }

    inline const TinyType* const* GetInterfaces() const
    {
        return reinterpret_cast<const TinyType* const*>(GetTypeHierarchy() + typeHierarchySize);
    }

    inline const packed_iterface_offset_t* GetInterfaceOffsets() const
    {
        return reinterpret_cast<const packed_iterface_offset_t*>(GetInterfaces() + interfacesSize);
    }

    // This is the number of elements (each of size uintptr_t) in the tiny type universe we used
    // for the packed interface offsets for this type.
    inline size_t NumberOfPackedInterfaceOffsetElements() const
    {
        return interfacesSize / NumberOfPackedInterfaceOffsetsPerElement + (interfacesSize % NumberOfPackedInterfaceOffsetsPerElement != 0 ? 1 : 0);
    }

    int GetId() const
    {
        return (int) * reinterpret_cast<const intptr_t*>(GetInterfaceOffsets() + NumberOfPackedInterfaceOffsetElements());
    }

#endif
} TinyType;

#define IL2CPP_BOXED_OBJECT_ALIGNMENT 8
#define IL2CPP_ALIGNED_OBJECT_SIZE ((sizeof(Il2CppObject) + IL2CPP_BOXED_OBJECT_ALIGNMENT - 1) & ~(IL2CPP_BOXED_OBJECT_ALIGNMENT - 1))

#if IL2CPP_MONO_DEBUGGER

typedef struct Il2CppInternalThread
{
    Il2CppObject obj;
    int lock_thread_id;
    void* handle;
    void* native_handle;
    Il2CppArray* cached_culture_info;
    Il2CppChar* name;
    int name_len;
    uint32_t state;
    Il2CppObject* abort_exc;
    int abort_state_handle;
    uint64_t tid;
    intptr_t debugger_thread;
    void** static_data;
    void* runtime_thread_info;
    Il2CppObject* current_appcontext;
    Il2CppObject* root_domain_thread;
    Il2CppArray* _serialized_principal;
    int _serialized_principal_version;
    void* appdomain_refs;
    int32_t interruption_requested;
    void* synch_cs;
    uint8_t threadpool_thread;
    uint8_t thread_interrupt_requested;
    int stack_size;
    uint8_t apartment_state;
    int critical_region_level;
    int managed_id;
    uint32_t small_id;
    void* manage_callback;
    void* interrupt_on_stop;
    intptr_t flags;
    void* thread_pinning_ref;
    void* abort_protected_block_count;
    int32_t priority;
    void* owned_mutexes;
    void * suspended;
    int32_t self_suspended;
    size_t thread_state;
    size_t unused2;
    void* last;
} Il2CppInternalThread;

// System.Threading.Thread
typedef struct Il2CppThread
{
    Il2CppObject obj;
    Il2CppInternalThread* internal_thread;
    Il2CppObject* start_obj;
    Il2CppException* pending_exception;
    Il2CppObject* principal;
    int32_t principal_version;
    Il2CppDelegate* delegate;
    Il2CppObject* executionContext;
    uint8_t executionContextBelongsToOuterScope;

#ifdef __cplusplus
    Il2CppInternalThread* GetInternalThread() const
    {
        return internal_thread;
    }

#endif //__cplusplus
} Il2CppThread;

#endif // IL2CPP_MONO_DEBUGGER

#endif // IL2CPP_TINY
