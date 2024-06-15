#include <string>
#include <algorithm>
#include "il2cpp-config.h"
#include "os/Thread.h"
#include "os/StackTrace.h"
#include "vm/StackTrace.h"
#include "os/ThreadLocalValue.h"
#include "os/Event.h"
#include "os/Image.h"
#include "Runtime.h"
#include "InsightGCStackTrace.h"
#if IL2CPP_TARGET_ANDROID && ENABLE_GC_PROFILE
#include <dlfcn.h>
#include <unwind.h>
#include <pthread.h>
#endif
#include "utils/Logging.h"

namespace il2cpp
{
    namespace vm
    {
        bool InsightGCStackTrace::s_Open = false;

#if ENABLE_GC_PROFILE

        os::ThreadLocalValue s_StackFrames;

        inline InsightGCStackTrace::GCStackTrace* GetStackTraceRaw()
        {
            InsightGCStackTrace::GCStackTrace* stacktrace;

            os::ErrorCode result = s_StackFrames.GetValue(reinterpret_cast<void**>(&stacktrace));
            Assert(result == os::kErrorCodeSuccess);

            return stacktrace;
        }


        void InsightGCStackTrace::InitializeStackTracesForCurrentThread()
        {
            if (GetStackTraceRaw() != NULL)
                return;

            InsightGCStackTrace::GCStackTrace* trace = new InsightGCStackTrace::GCStackTrace();

            os::ErrorCode result = s_StackFrames.SetValue(trace);
            Assert(result == os::kErrorCodeSuccess);
        }

        void InsightGCStackTrace::CleanupStackTracesForCurrentThread()
        {
            InsightGCStackTrace::GCStackTrace* trace = GetStackTraceRaw();

            if (trace == NULL)
                return;

            delete trace;

            os::ErrorCode result = s_StackFrames.SetValue(NULL);
            Assert(result == os::kErrorCodeSuccess);
        }

        void InsightGCStackTrace::PushMethod(int method)
        {
            InsightGCStackTrace::GCStackTrace* trace = NULL;

            if ((trace = GetStackTraceRaw()) == NULL)
                return;

            trace->push_back(method);
        }

        void InsightGCStackTrace::PopMethod()
        {
            InsightGCStackTrace::GCStackTrace* trace = NULL;

            if ((trace = GetStackTraceRaw()) == NULL)
                return;

            if (trace->size() > 0)
            {
                trace->pop_back();
            }
        }
#if IL2CPP_TARGET_ANDROID
        struct NativeStackTraceContext
        {
            unsigned long long* stack;
            short* mod;
            int count;
            int maxCount;
        };
        static bool GetStackFramesCallback(Il2CppMethodPointer frame, void* context)
        {

            const char* filename = NULL;
            const char* symbol = "";
            void* baseAddr = NULL;
            Dl_info info;
            if (dladdr(reinterpret_cast<void*>(frame), &info))
            {
                if (info.dli_sname)
                    symbol = info.dli_sname;
                if (info.dli_fname)
                    filename = info.dli_fname;
                if (info.dli_fbase)
                    baseAddr = info.dli_fbase;
            }
            NativeStackTraceContext* trace = static_cast<NativeStackTraceContext*>(context);
            if (trace->count >= trace->maxCount)
                return false;

#ifdef __aarch64__   // arm 64Î»ºê https://developer.android.com/ndk/guides/cpu-features?hl=zh-cn
            trace->stack[trace->count] = (unsigned long long)frame - (unsigned long long)baseAddr;
#else
            trace->stack[trace->count] = (unsigned int)frame - (unsigned int)baseAddr;
#endif
            trace->mod[trace->count] = 0;

            if (filename)
            {
                const char* const slash = strrchr(filename, '/');
                if (slash && strcmp(slash + 1, "libunity.so") == 0)
                {
                    trace->mod[trace->count] = 1;
                }
                else if (slash && strcmp(slash + 1, "libGameCore.so") == 0)
                {
                    trace->mod[trace->count] = 2;
                }
                else if (slash && strcmp(slash + 1, "libil2cpp.so") == 0)
                {
                    trace->mod[trace->count] = 3;
                }
            }

            trace->count++;

            return true;
        }

#endif
        int InsightGCStackTrace::GetStackTrace(unsigned long long* stack, short* mod, int count)
        {
            InsightGCStackTrace::GCStackTrace* trace = GetStackTraceRaw();
            if (trace == NULL)
            {
                return 0;
            }
            int num = count > trace->size() ? trace->size() : count;
#if IL2CPP_TARGET_ANDROID
            if (num <= 4)
            {

                NativeStackTraceContext context;
                context.stack = stack;
                context.mod = mod;
                context.count = 0;
                context.maxCount = count;
                os::StackTrace::WalkStack(&GetStackFramesCallback, &context, os::StackTrace::kFirstCalledToLastCalled);
                return context.count;
            }
            else
#endif
            {
                for (int i = 0; i < num; ++i)
                {
                    stack[i] = (*trace)[i];
                    mod[i] = -1;
                }
                return num;
            }
        }

#else

        void InsightGCStackTrace::InitializeStackTracesForCurrentThread()
        {

        }

        void InsightGCStackTrace::CleanupStackTracesForCurrentThread()
        {

        }

        void InsightGCStackTrace::PushMethod(int method)
        {

        }

        void InsightGCStackTrace::PopMethod()
        {

        }

        int InsightGCStackTrace::GetStackTrace(unsigned long long* stack, short* module, int count)
        {
            return 0;
        }
#endif
    }
}
