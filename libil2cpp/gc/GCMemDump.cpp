#include "il2cpp-config.h"
#include "GCMemDump.h"

#if IL2CPP_GC_MEM_DUMP

#include "il2cpp-object-internals.h"
#include "os/Event.h"
#include "os/Mutex.h"
#include "os/Semaphore.h"
#include "os/Thread.h"
#include "utils/Il2CppHashMap.h"
#include "utils/HashUtils.h"
#include "vm/Class.h"
#include "vm/Exception.h"
#include "vm/Runtime.h"
#include "vm/Thread.h"
#include "gc/gc_wrapper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "vm-utils/NativeSymbol.h"
#include <stdio.h>

#define HAS_POSIX_BACKTRACE (IL2CPP_TARGET_IOS || IL2CPP_TARGET_ANDROID)
#if HAS_POSIX_BACKTRACE

#if IL2CPP_TARGET_IOS
#include <execinfo.h>  // for backtrace
#elif IL2CPP_TARGET_ANDROID
#include <unwind.h>   // for backtrace
#endif

#include <dlfcn.h>     // for dladdr
#include <cxxabi.h>    // for __cxa_demangle

#endif

#define MAX_ALLOCLARGE_SIZE (1024*128)
#define SKIP_STACK_RECORD 1 //3
#define MAX_STACK_RECORD_FRAMES 20


#ifdef __cplusplus
extern "C" {
#endif

void _SetiOSMemoryMonitorAlloc(bool isActive);
void _DumpiOSMemory(const char *mallocDumpfilename);

//gc
void InitGCDump(void* func);
void SetGCMonitorAlloc(bool isActive);
void AddGCRecord(void* ptr, size_t size);
void DeleteGCRecord(void* ptr);
void DumpGCMememory(const char *mallocDumpfilename);
void DumpLargeGCMememory(const char *mallocDumpfilename);

#ifdef __cplusplus
} // extern "C"

#endif

namespace il2cpp
{
	namespace gc
	{
        static bool isDumping = false;
        static bool sDelPtrHash = true;

		//static bool sMonitorAlloc = false;

		void GCMemDump::SetMonitorAlloc(bool isActive)
		{
			SetGCMonitorAlloc(isActive);
		}

		size_t GetGCSize(void* addr, size_t size)
		{
			return GC_size(addr);
		}

		void GCMemDump::InitGCFunc()
		{
			InitGCDump((void*)GetGCSize);
		}

        void GCMemDump::RegMallocPtr(void * newPtr, size_t newSize, bool isReg)
        {
            if (isDumping)
            {
                return;
            }

            AddGCRecord(newPtr, newSize);


            if (isReg)
            {
                GC_REGISTER_FINALIZER_NO_ORDER(newPtr, &GCMemDump::RunFinalizerForMalloc, NULL, NULL, NULL);
            }
        }

        void GCMemDump::DumpGCMemInfo(const char* filename, bool isPrintLargeAlloc)
        {
            isDumping = true;
            do
            {
			DumpGCMememory(filename);
			if (strlen(filename) < 1024)
			{
				char large_file_name[1024] = {0};
				strcpy(large_file_name, filename);
				int last_index = strlen(filename);
				large_file_name[last_index] = 0;
				large_file_name[last_index - 1] = 'r';
				large_file_name[last_index - 2] = 'a';
				large_file_name[last_index - 3] = 'l';
				large_file_name[last_index - 4] = '.';
				DumpLargeGCMememory((const char*)large_file_name);

			}
			else
			{
				char large_file_name[2048] = {0};
				strcpy(large_file_name, filename);
				int last_index = strlen(filename);
				large_file_name[last_index] = 0;
				large_file_name[last_index - 1] = 'r';
				large_file_name[last_index - 2] = 'a';
				large_file_name[last_index - 3] = 'l';
				large_file_name[last_index - 4] = '.';
				DumpLargeGCMememory((const char*)large_file_name);
			}
            } while (false);

            isDumping = false;
        }

        static void _OnCallbackPtrNoUse(void *ptr)
        {
            if (!sDelPtrHash)
            {
                return;
            }


            if (isDumping)
            {
                return;
            }

            DeleteGCRecord(ptr);
        }


        void GCMemDump::RunFinalizerForMalloc(void * obj, void * data)
        {
            _OnCallbackPtrNoUse(obj);
        }


        void GCMemDump::SetCallbackDelMask(bool isDel)
        {
            sDelPtrHash = isDel;
        }
    } // namespace gc
} // namespace il2cpp
#else
#pragma message("IL2CPP_GC_MEM_DUMP == 0")
#endif

