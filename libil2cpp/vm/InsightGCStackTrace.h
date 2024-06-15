#pragma once

#include "os/Mutex.h"
#include <vector>
#include <map>

namespace il2cpp
{
    namespace vm
    {
        class InsightGCStackTrace
        {
public:
            enum {
                NONE,
                ALLOCATION_ARRAY_NEWSPECIFIC = 1,
                ALLOCATION_ARRAY_NEWFULL = 2,
                ALLOCATION_OBJECT_CLONE = 3,
                ALLOCATION_OBJECT_NEWSPECIFIC = 4,
                ALLOCATION_STRING_NEWSIZE = 5,
                ALLOCATION_OBJECT_BOX = 6,
                ALLOCATION_ARRAY_NEW = 7,
            };

            struct InsightGCProfilerInnerSentry
            {
                InsightGCProfilerInnerSentry(int methodIndex)
                {
                    il2cpp::vm::InsightGCStackTrace::PushMethod(methodIndex);
                }
                ~InsightGCProfilerInnerSentry()
                {
                    il2cpp::vm::InsightGCStackTrace::PopMethod();
                }
            };

            typedef std::vector<int> GCStackTrace;

            static void InitializeStackTracesForCurrentThread();
            static void CleanupStackTracesForCurrentThread();

            static void PushMethod(int method);
            static void PopMethod();

            static int GetStackTrace(unsigned long long* stack, short* module, int count);

            static inline void OpenStackTrace() { s_Open = true; }

            static inline void CloseStackTrace() { s_Open = false; }
#if ENABLE_GC_PROFILE
            static inline bool IsStackTraceEnable() { return true; }
#else
            static inline bool IsStackTraceEnable() { return false; }
#endif
        private:
            static bool s_Open;
        };

    }
}
