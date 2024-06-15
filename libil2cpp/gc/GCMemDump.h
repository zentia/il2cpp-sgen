#pragma once

#include <stdio.h>

//#define IL2CPP_GC_MEM_DUMP (1)

namespace il2cpp
{
    namespace gc
    {
        typedef void* (*MallocFuncType)(size_t __size);
        typedef void  (*FreeFuncType)(void *ptr);

        class GCMemDump
        {
        public:
            //call from project
            static void DumpGCMemInfo(const char* filename, bool isPrintLargeAlloc);
			static void SetMonitorAlloc(bool isActive);

        public:
            static void RegMallocPtr(void* newPtr, size_t newSize, bool isReg);
            static void RunFinalizerForMalloc(void *obj, void *data);
            static void SetCallbackDelMask(bool isDel);
            static void InitGCFunc();

        private:
            GCMemDump();
        };
    } /* namespace vm */
} /* namespace il2cpp */


#if IL2CPP_GC_MEM_DUMP
//#include "Export.h"


#define INIT_GCDUMP il2cpp::gc::GCMemDump::InitGCFunc();
#define BIND_MALLOC(obj, size, isReg) do { il2cpp::gc::GCMemDump::RegMallocPtr(obj, size,isReg); } while (false)
#define RUNFINALIZERFORMALLOC(obj, data) do { il2cpp::gc::GCMemDump::RunFinalizerForMalloc(obj, data); } while (false)
#define REGISTERFINALIZERFORMALLOC(callback) do { if (callback == NULL) { callback = &(il2cpp::gc::GCMemDump::RunFinalizerForMalloc); } } while (false)
#define CHECKHASFINALIZER(oldCb) (oldCb != NULL && oldCb != &GCMemDump::RunFinalizerForMalloc)

#define DUMPGCMEMINFO(filename, isPrintLargeAlloc) do { il2cpp::gc::GCMemDump::DumpGCMemInfo(filename, isPrintLargeAlloc); } while (false)
#define SETCALLBACKDELMASK(isDel) do { il2cpp::gc::GCMemDump::SetCallbackDelMask(isDel); } while (false)

#define SETMONITORALLOC(isActive) do { il2cpp::gc::GCMemDump::SetMonitorAlloc(isActive); } while (false)

#else

#define INIT_GCDUMP 
#define BIND_MALLOC(obj, size, isReg)
#define RUNFINALIZERFORMALLOC(obj, data)
#define REGISTERFINALIZERFORMALLOC(callback)
#define CHECKHASFINALIZER(oldCb) (oldCb != NULL)

#define DUMPGCMEMINFO(filename, isPrintLargeAlloc)
#define SETCALLBACKDELMASK(isDel)

#define SETMONITORALLOC(isActive)
#endif
