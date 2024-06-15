#include "il2cpp-config.h"
#include "StackTrace.h"
#include "il2cpp-object-internals.h"
#include "os/Event.h"
#include "os/StackTrace.h"
#include "os/Thread.h"
#include "os/ThreadLocalValue.h"
#include "os/Image.h"
#include "vm/Method.h"
#include "vm/Thread.h"
#include "vm/Type.h"
#include "vm/Class.h"
#include "vm-utils/Debugger.h"
#include "vm-utils/NativeSymbol.h"
#include "vm-utils/DebugSymbolReader.h"
#include "vm-utils/Debugger.h"
#include "utils/Logging.h"

#include <map>
#include <cstdio>

#if IL2CPP_TARGET_ANDROID || IL2CPP_TARGET_IOS
#include <dlfcn.h>
#endif

namespace il2cpp
{
namespace vm
{
#if IL2CPP_ENABLE_STACKTRACES

    class CachedInfo
    {
        int32_t m_depth;
        const void* m_stackPointer;
    public:
        CachedInfo() : m_depth(INT_MAX), m_stackPointer(NULL) {}
        void Update(int32_t depth, const void *stackPointer)
        {
            m_depth = depth;
            m_stackPointer = stackPointer;
        }

        bool CheckCondition(int32_t depth, const void *stackPointer) const
        {
            // We can use cached value if stack pointer is the same and not NULL, and 'depth' has been incremented since previous call
            return m_stackPointer != NULL && stackPointer == m_stackPointer && depth - 1 == m_depth;
        }
    };

    class MethodStack
    {
    protected:
        os::ThreadLocalValue s_StackFrames;
        os::ThreadLocalValue s_StoredCachedInfo;

        inline StackFrames* GetStackFramesRaw()
        {
            StackFrames* stackFrames = NULL;

            os::ErrorCode result = s_StackFrames.GetValue(reinterpret_cast<void**>(&stackFrames));
            Assert(result == os::kErrorCodeSuccess);

            return stackFrames;
        }

        inline CachedInfo* GetStoredCachedInfoRaw()
        {
            CachedInfo* storedCachedInfo = NULL;

            os::ErrorCode result = s_StoredCachedInfo.GetValue(reinterpret_cast<void**>(&storedCachedInfo));
            Assert(result == os::kErrorCodeSuccess);

            return storedCachedInfo;
        }

    public:
        inline void InitializeForCurrentThread()
        {
            if (GetStackFramesRaw() != NULL)
                return;

            StackFrames* stackFrames = new StackFrames();
            stackFrames->reserve(64);

            os::ErrorCode result = s_StackFrames.SetValue(stackFrames);
            Assert(result == os::kErrorCodeSuccess);

            CachedInfo* cachedInfo = new CachedInfo();
            result = s_StoredCachedInfo.SetValue(cachedInfo);
            Assert(result == os::kErrorCodeSuccess);
        }

        inline void CleanupForCurrentThread()
        {
            StackFrames* frames = GetStackFramesRaw();

            if (frames == NULL)
                return;

            delete frames;

            CachedInfo* cachedInfo = GetStoredCachedInfoRaw();

            if (cachedInfo == NULL)
                return;

            delete cachedInfo;

            os::ErrorCode result = s_StackFrames.SetValue(NULL);
            Assert(result == os::kErrorCodeSuccess);
            result = s_StoredCachedInfo.SetValue(NULL);
            Assert(result == os::kErrorCodeSuccess);
        }
    };

#if IL2CPP_ENABLE_STACKTRACE_SENTRIES

    class StacktraceSentryMethodStack : public MethodStack
    {
    public:
        inline const StackFrames* GetStackFrames()
        {
            return GetStackFramesRaw();
        }

        inline const StackFrames* GetCachedStackFrames(int32_t depth, const void* stackPointer)
        {
            return GetStackFrames();
        }

        inline bool GetStackFrameAt(int32_t depth, Il2CppStackFrameInfo& frame)
        {
            const StackFrames& frames = *GetStackFramesRaw();

            if (static_cast<int>(frames.size()) + depth < 1)
                return false;

            frame = frames[frames.size() - 1 + depth];
            return true;
        }

        inline void PushFrame(Il2CppStackFrameInfo& frame)
        {
            GetStackFramesRaw()->push_back(frame);
        }

        inline void PopFrame()
        {
            StackFrames* stackFrames = GetStackFramesRaw();
            stackFrames->pop_back();
        }

        inline const void* GetStackPointer()
        {
            return nullptr;
        }
    };

#endif // IL2CPP_ENABLE_STACKTRACE_SENTRIES

#if IL2CPP_ENABLE_NATIVE_STACKTRACES

#if IL2CPP_MONO_DEBUGGER
    class DebuggerMethodStack : public MethodStack
    {
    public:
        inline const StackFrames* GetStackFrames()
        {
            StackFrames* stackFrames = GetStackFramesRaw();
            if (stackFrames == NULL)
                return stackFrames;
            stackFrames->clear();

            utils::Debugger::GetStackFrames(stackFrames);

            return stackFrames;
        }

        inline const StackFrames* GetStackFramesWithLog()
        {
            return GetStackFrames();
        }

        inline const StackFrames* GetCachedStackFrames(int32_t depth, const void* stackPointer)
        {
            CachedInfo* cachedInfo = GetStoredCachedInfoRaw();
            const StackFrames* stackFrames = cachedInfo->CheckCondition(depth, stackPointer) ? GetStackFramesRaw() : GetStackFrames();
            cachedInfo->Update(depth, stackPointer);
            return stackFrames;
        }

        inline bool GetStackFrameAt(int32_t depth, Il2CppStackFrameInfo& frame)
        {
            const StackFrames& frames = *GetStackFrames();

            if (static_cast<int>(frames.size()) + depth < 1)
                return false;

            frame = frames[frames.size() - 1 + depth];
            return true;
        }

        inline void PushFrame(Il2CppStackFrameInfo& frame)
        {
        }

        inline void PopFrame()
        {
        }

        inline const void* GetStackPointer()
        {
            return nullptr;
        }
    };
#else
    class NativeMethodStack : public MethodStack
    {   
        static bool GetStackFramesCallback(Il2CppMethodPointer frame, void* context)
        {
            const MethodInfo* method = il2cpp::utils::NativeSymbol::GetMethodFromNativeSymbol(frame);
            StackFrames* stackFrames = static_cast<StackFrames*>(context);

            if (method != NULL)
            {
                bool frames_added = il2cpp::utils::DebugSymbolReader::AddStackFrames(reinterpret_cast<void*>(frame), stackFrames);

                if (!frames_added)
                {
                    Il2CppStackFrameInfo frameInfo = { 0 };
                    frameInfo.method = method;
                    frameInfo.raw_ip = reinterpret_cast<uintptr_t>(frame) - reinterpret_cast<uintptr_t>(os::Image::GetImageBase());
                    stackFrames->push_back(frameInfo);
                }
            }

            return true;
        }

        static bool GetStackFramesCallbackAllInOnce(Il2CppMethodPointer* frames, size_t frameCount, void* context, int walkOrder)
        {
            return GetStackFramesCallbackInternal(frames, frameCount, context, walkOrder, false);
        }

        static bool GetStackFramesCallbackWithLogAllInOnce(Il2CppMethodPointer* frames, size_t frameCount, void* context, int walkOrder)
        {
            return GetStackFramesCallbackInternal(frames, frameCount, context, walkOrder, true);
        }

        static bool GetStackFramesCallbackInternal(Il2CppMethodPointer* frames, size_t frameCount, void* context, int walkOrder, bool enableLog)
        {
            StackFrames* stackFrames = static_cast<StackFrames*>(context);
            static const MethodInfo* exceptionMethod = Class::GetMethodFromName(il2cpp_defaults.exception_class, "ToString", 0);
            bool hasLocalParseFlag = il2cpp::vm::StackTrace::GetParseFromLocalFlag();
            for (size_t i = 0; i < frameCount; i++)
            {
                const size_t index = (walkOrder == 0 ? (frameCount - i - 1) : i);
                Il2CppMethodPointer frame = frames[index];

#if IL2CPP_ENABLE_STACKTRACE_PC_ADDRESS
                // 安卓和IOS平台需要输出原始调用栈
                const char* filename = "";
                const char* symbol = "";
                void* baseAddr = NULL;

#if IL2CPP_TARGET_IOS
                uintptr_t imageBase = reinterpret_cast<uintptr_t>(os::Image::GetImageBase());
                uintptr_t current = reinterpret_cast<uintptr_t>(frame);
                uintptr_t end = imageBase + reinterpret_cast<uintptr_t>(os::Image::GetTextSectionLength());
                uintptr_t start = imageBase;
                if (current > start && current <= end)
                {
                    filename = "UnityFramework";//filename_str	std::string	"UnityFramework"	
                    baseAddr = (void*)imageBase;
                }
                else
                {
#endif
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
#if IL2CPP_TARGET_IOS
                }
#endif

                //printf("[Dl_info] %s %s %llu\n", filename, symbol, (unsigned long long)baseAddr);

                StackFrames* stackFrames = static_cast<StackFrames*>(context);
                Il2CppStackFrameInfo frameInfo = { 0 };
                frameInfo.method = exceptionMethod;
                
                if (hasLocalParseFlag == true)
                {
                    const MethodInfo* method = il2cpp::utils::NativeSymbol::GetMethodFromNativeSymbol(frame);
                    if (method != NULL)
                    {
                        frameInfo.method = method;
                    }
                }
                
                frameInfo.raw_ip = (uintptr_t)frame - (uintptr_t)baseAddr;

                // record .so file
                std::string filename_str(filename);
                size_t pos = filename_str.find_last_of('/');
                if (pos != std::string::npos) {
                    filename_str = filename_str.substr(pos + 1);
                }
                strcpy(frameInfo.image_name, filename_str.c_str());
                stackFrames->push_back(frameInfo);
#else
                // 非安卓和ios平台
                const MethodInfo* method = il2cpp::utils::NativeSymbol::GetMethodFromNativeSymbol(frame);
                if (method != NULL)
                {
                    Il2CppStackFrameInfo frameInfo = { 0 };
                    frameInfo.method = method;
                    frameInfo.raw_ip = 0;
                    stackFrames->push_back(frameInfo);
                }
#endif
            }

            if (enableLog) 
            {
#if IL2CPP_TARGET_ANDROID         
                size_t count = stackFrames->size();
                for (size_t i = 0; i < count; i++)
                {
                    const size_t index = (walkOrder == 0 ? (count - i - 1) : i);
                    Il2CppStackFrameInfo frameInfo = (*stackFrames)[index];
                    IL2CPP_TRACE("#%02d pc %llx %s\n", i, (unsigned long long)frameInfo.raw_ip, frameInfo.image_name);
                }           
#endif 
            }

            return true;
        }

        struct GetStackFrameAtContext
        {
            int32_t currentDepth;
            const MethodInfo* method;
        };

        static bool GetStackFrameAtCallback(Il2CppMethodPointer frame, void* context)
        {
            const MethodInfo* method = il2cpp::utils::NativeSymbol::GetMethodFromNativeSymbol(frame);
            GetStackFrameAtContext* ctx = static_cast<GetStackFrameAtContext*>(context);

            if (method != NULL)
            {
                if (ctx->currentDepth == 0)
                {
                    ctx->method = method;
                    return false;
                }

                ctx->currentDepth++;
            }

            return true;
        }

    public:
        inline const StackFrames* GetStackFrames()
        {
            StackFrames* stackFrames = GetStackFramesRaw();
            if (stackFrames == NULL)
                return stackFrames;
            stackFrames->clear();

            // os::StackTrace::WalkStack(&NativeMethodStack::GetStackFramesCallback, stackFrames, os::StackTrace::kFirstCalledToLastCalled);
            os::StackTrace::WalkStackAllInOnce(&NativeMethodStack::GetStackFramesCallbackAllInOnce, stackFrames, os::StackTrace::kFirstCalledToLastCalled);

            return stackFrames;
        }

        inline const StackFrames* GetStackFramesWithLog()
        {
            StackFrames* stackFrames = GetStackFramesRaw();
            if (stackFrames == NULL)
                return stackFrames;
            stackFrames->clear();

            os::StackTrace::WalkStackAllInOnce(&NativeMethodStack::GetStackFramesCallbackWithLogAllInOnce, stackFrames, os::StackTrace::kFirstCalledToLastCalled);

            return stackFrames;
        }

        // Avoiding calling GetStackFrames() method for the same stack trace with incremented 'depth' value
        inline const StackFrames* GetCachedStackFrames(int32_t depth, const void* stackPointer)
        {
            CachedInfo* cachedInfo = GetStoredCachedInfoRaw();
            const StackFrames* stackFrames = cachedInfo->CheckCondition(depth, stackPointer) ? GetStackFramesRaw() : GetStackFrames();
            cachedInfo->Update(depth, stackPointer);
            return stackFrames;
        }

        inline bool GetStackFrameAt(int32_t depth, Il2CppStackFrameInfo& frame)
        {
            GetStackFrameAtContext context = { depth, NULL };

            os::StackTrace::WalkStack(&NativeMethodStack::GetStackFrameAtCallback, &context, os::StackTrace::kLastCalledToFirstCalled);

            if (context.method != NULL)
            {
                frame.method = context.method;
                return true;
            }

            return false;
        }

        inline void PushFrame(Il2CppStackFrameInfo& frame)
        {
        }

        inline void PopFrame()
        {
        }

        // Returns SP value or nullptr if not implemented
        inline const void* GetStackPointer()
        {
            return os::StackTrace::GetStackPointer();
        }
    };
#endif // IL2CPP_MONO_DEBUGGER

#endif // IL2CPP_ENABLE_NATIVE_STACKTRACES

#else

    static StackFrames s_EmptyStack;

    class NoOpMethodStack
    {
    public:
        inline void InitializeForCurrentThread()
        {
        }

        inline void CleanupForCurrentThread()
        {
        }

        inline const StackFrames* GetStackFrames()
        {
            return &s_EmptyStack;
        }

        inline const StackFrames* GetCachedStackFrames(int32_t depth, const void* stackPointer)
        {
            return GetStackFrames();
        }

        inline bool GetStackFrameAt(int32_t depth, Il2CppStackFrameInfo& frame)
        {
            return false;
        }

        inline void PushFrame(Il2CppStackFrameInfo& frame)
        {
        }

        inline void PopFrame()
        {
        }

        inline const void* GetStackPointer()
        {
            return nullptr;
        }
    };

#endif // IL2CPP_ENABLE_STACKTRACES

#if IL2CPP_ENABLE_STACKTRACES

#if IL2CPP_ENABLE_STACKTRACE_SENTRIES

    StacktraceSentryMethodStack s_MethodStack;

#elif IL2CPP_ENABLE_NATIVE_STACKTRACES

#if IL2CPP_MONO_DEBUGGER
    DebuggerMethodStack s_MethodStack;
#else
    NativeMethodStack s_MethodStack;
#endif

#endif

#else

    NoOpMethodStack s_MethodStack;

#endif // IL2CPP_ENABLE_STACKTRACES

    bool s_parse_from_local = false;

// Current thread functions

    void StackTrace::InitializeStackTracesForCurrentThread()
    {
        s_MethodStack.InitializeForCurrentThread();
    }

    void StackTrace::CleanupStackTracesForCurrentThread()
    {
        s_MethodStack.CleanupForCurrentThread();
    }

    const StackFrames* StackTrace::GetStackFrames()
    {
        return s_MethodStack.GetStackFrames();
    }

    const StackFrames* StackTrace::GetStackFramesWithLog()
    {
        return s_MethodStack.GetStackFramesWithLog();
    }

    const StackFrames* StackTrace::GetCachedStackFrames(int32_t depth)
    {
        return s_MethodStack.GetCachedStackFrames(depth, GetStackPointer());
    }

    bool StackTrace::GetStackFrameAt(int32_t depth, Il2CppStackFrameInfo& frame)
    {
        Assert(depth <= 0 && "Frame depth must be 0 or less");
        return s_MethodStack.GetStackFrameAt(depth, frame);
    }

    void StackTrace::WalkFrameStack(Il2CppFrameWalkFunc callback, void* context)
    {
        const StackFrames& frames = *GetStackFrames();

        for (StackFrames::const_iterator it = frames.begin(); it != frames.end(); it++)
            callback(&*it, context);
    }

#if IL2CPP_TINY_DEBUGGER

    static std::map<const MethodInfo*, std::string> s_MethodNames;

    static std::string MethodNameFor(const MethodInfo* method)
    {
        auto cachedMethodNameEntry = s_MethodNames.find(method);
        if (cachedMethodNameEntry != s_MethodNames.end())
            return cachedMethodNameEntry->second;

        std::string methodName;
        methodName += vm::Method::GetFullName(method);
        methodName += " (";
        uint32_t numberOfParameters = vm::Method::GetParamCount(method);
        for (uint32_t j = 0; j < numberOfParameters; ++j)
        {
            const Il2CppType* parameterType = vm::Method::GetParam(method, j);
            methodName += vm::Type::GetName(parameterType, IL2CPP_TYPE_NAME_FORMAT_FULL_NAME);
            if (j != numberOfParameters - 1)
                methodName += ", ";
        }
        methodName += ")";

        s_MethodNames[method] = methodName;

        return methodName;
    }

    const int s_StackTraceSize = 4096;
    static char s_StackTrace[s_StackTraceSize];
    static int s_StackTraceOffset = 0;

    static void AppendToStackTrace(const char* value)
    {
        if (s_StackTraceOffset < s_StackTraceSize)
            s_StackTraceOffset += snprintf(s_StackTrace + s_StackTraceOffset, s_StackTraceSize - s_StackTraceOffset, "%s", value);
    }

    const char* StackTrace::GetStackTrace()
    {
        // Since a pointer to a static buffer is used to store the stack trace, only
        // once thread should use it. Tiny does not support managed code on non-main
        // threads, so there is no need to use a thread local buffer here. Protect this
        // from access on multiple threads by not returning anyting on non-main threads.
        if (vm::Thread::Current() != vm::Thread::Main())
            return NULL;

        const StackFrames* frames = s_MethodStack.GetStackFrames();

        const size_t numberOfFramesToSkip = 1;
        int startFrame = (int)frames->size() - 1 - numberOfFramesToSkip;

        s_StackTraceOffset = 0;
        for (int i = startFrame; i > 0; i--)
        {
            if (i == startFrame)
                AppendToStackTrace("at ");
            else
                AppendToStackTrace("  at ");
            Il2CppStackFrameInfo stackFrame = (*frames)[i];
            AppendToStackTrace(MethodNameFor(stackFrame.method).c_str());
            if (stackFrame.filePath != NULL)
            {
                AppendToStackTrace(" in ");
                AppendToStackTrace(stackFrame.filePath);
                AppendToStackTrace(":");
                AppendToStackTrace(std::to_string(stackFrame.sourceCodeLineNumber).c_str());
            }
            if (i != 1)
                AppendToStackTrace("\n");
        }

        return s_StackTrace;
    }

#endif

    void StackTrace::PushFrame(Il2CppStackFrameInfo& frame)
    {
        s_MethodStack.PushFrame(frame);
    }

    void StackTrace::PopFrame()
    {
        s_MethodStack.PopFrame();
    }

    const void* StackTrace::GetStackPointer()
    {
        return s_MethodStack.GetStackPointer();
    }

// Remote thread functions

    struct GetThreadFrameAtContext
    {
        il2cpp::os::Event apcDoneEvent;
        int32_t depth;
        Il2CppStackFrameInfo* frame;
        bool hasResult;
    };

    struct WalkThreadFrameStackContext
    {
        il2cpp::os::Event apcDoneEvent;
        Il2CppFrameWalkFunc callback;
        void* userContext;
    };

    struct GetThreadStackDepthContext
    {
        il2cpp::os::Event apcDoneEvent;
        int32_t stackDepth;
    };

    struct GetThreadTopFrameContext
    {
        il2cpp::os::Event apcDoneEvent;
        Il2CppStackFrameInfo* frame;
        bool hasResult;
    };

    static void STDCALL GetThreadFrameAtCallback(void* context)
    {
        GetThreadFrameAtContext* ctx = static_cast<GetThreadFrameAtContext*>(context);

        ctx->hasResult = StackTrace::GetStackFrameAt(ctx->depth, *ctx->frame);
        ctx->apcDoneEvent.Set();
    }

    bool StackTrace::GetThreadStackFrameAt(Il2CppThread* thread, int32_t depth, Il2CppStackFrameInfo& frame)
    {
#if IL2CPP_ENABLE_STACKTRACES
        GetThreadFrameAtContext apcContext;

        apcContext.depth = depth;
        apcContext.frame = &frame;

        thread->GetInternalThread()->handle->QueueUserAPC(GetThreadFrameAtCallback, &apcContext);
        apcContext.apcDoneEvent.Wait();

        return apcContext.hasResult;
#else
        return false;
#endif
    }

    static void STDCALL WalkThreadFrameStackCallback(void* context)
    {
        WalkThreadFrameStackContext* ctx = static_cast<WalkThreadFrameStackContext*>(context);

        StackTrace::WalkFrameStack(ctx->callback, ctx->userContext);
        ctx->apcDoneEvent.Set();
    }

    void StackTrace::WalkThreadFrameStack(Il2CppThread* thread, Il2CppFrameWalkFunc callback, void* context)
    {
#if IL2CPP_ENABLE_STACKTRACES
        WalkThreadFrameStackContext apcContext;

        apcContext.callback = callback;
        apcContext.userContext = context;

        thread->GetInternalThread()->handle->QueueUserAPC(WalkThreadFrameStackCallback, &apcContext);
        apcContext.apcDoneEvent.Wait();
#endif
    }

    static void STDCALL GetThreadStackDepthCallback(void* context)
    {
        GetThreadStackDepthContext* ctx = static_cast<GetThreadStackDepthContext*>(context);

        ctx->stackDepth = static_cast<int32_t>(StackTrace::GetStackDepth());
        ctx->apcDoneEvent.Set();
    }

    int32_t StackTrace::GetThreadStackDepth(Il2CppThread* thread)
    {
#if IL2CPP_ENABLE_STACKTRACES
        GetThreadStackDepthContext apcContext;

        thread->GetInternalThread()->handle->QueueUserAPC(GetThreadStackDepthCallback, &apcContext);
        apcContext.apcDoneEvent.Wait();

        return apcContext.stackDepth;
#else
        return 0;
#endif
    }

    static void STDCALL GetThreadTopFrameCallback(void* context)
    {
        GetThreadTopFrameContext* ctx = static_cast<GetThreadTopFrameContext*>(context);

        ctx->hasResult = StackTrace::GetTopStackFrame(*ctx->frame);
        ctx->apcDoneEvent.Set();
    }

    bool StackTrace::GetThreadTopStackFrame(Il2CppThread* thread, Il2CppStackFrameInfo& frame)
    {
#if IL2CPP_ENABLE_STACKTRACES
        GetThreadTopFrameContext apcContext;
        apcContext.frame = &frame;

        thread->GetInternalThread()->handle->QueueUserAPC(GetThreadTopFrameCallback, &apcContext);
        apcContext.apcDoneEvent.Wait();

        return apcContext.hasResult;
#else
        return false;
#endif
    }

    void StackTrace::SetParseFromLocalFlag(bool parseFromLocal)
    {
        s_parse_from_local = parseFromLocal;
    }
    bool StackTrace::GetParseFromLocalFlag()
    {
        return s_parse_from_local;
    }
}
}
