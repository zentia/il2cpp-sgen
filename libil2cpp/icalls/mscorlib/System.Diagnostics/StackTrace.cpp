#if !IL2CPP_TINY
#include "il2cpp-config.h"
#include "il2cpp-class-internals.h"
#include "il2cpp-object-internals.h"
#include "gc/WriteBarrier.h"
#include "vm/Array.h"
#include "vm/Object.h"
#include "vm/Reflection.h"
#include "vm/String.h"
#include "icalls/mscorlib/System.Diagnostics/StackTrace.h"
#include "vm-utils/DebugSymbolReader.h"
#include "utils/Logging.h"

namespace il2cpp
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
namespace Diagnostics
{
    static Il2CppArray* GetTraceInternal(Il2CppArray* trace_ips, int32_t skip, bool need_file_info)
    {
        /* Exception is not thrown yet */
        if (trace_ips == NULL)
            return vm::Array::New(il2cpp_defaults.stack_frame_class, 0);

        int len = vm::Array::GetLength(trace_ips);
        Il2CppArray* stackFrames = vm::Array::New(il2cpp_defaults.stack_frame_class, len > skip ? len - skip : 0);

        for (int i = skip; i < len; i++)
        {
            Il2CppStackFrame* stackFrame = NULL;

            if (utils::DebugSymbolReader::DebugSymbolsAvailable())
            {
                stackFrame = il2cpp_array_get(trace_ips, Il2CppStackFrame*, i);
            }
            else
            {
                stackFrame = (Il2CppStackFrame*)vm::Object::New(il2cpp_defaults.stack_frame_class);
                MethodInfo* method = il2cpp_array_get(trace_ips, MethodInfo*, i);

                IL2CPP_OBJECT_SETREF(stackFrame, method, vm::Reflection::GetMethodObject(method, NULL));
            }

            il2cpp_array_setref(stackFrames, i, stackFrame);
        }

        return stackFrames;
    }

    static Il2CppArray* GetTraceInternalWithLog(Il2CppException *exc, int32_t skip, bool need_file_info)
    {
        Il2CppArray *trace_ips = exc->trace_ips;
		Il2CppArray *pc_stack_frames = (Il2CppArray *)exc->_data;
        if (trace_ips == NULL)
            return vm::Array::New(il2cpp_defaults.stack_frame_class, 0);

        int len = vm::Array::GetLength(trace_ips);
        int pc_len = pc_stack_frames != NULL ? vm::Array::GetLength(pc_stack_frames) : 0;
        Il2CppArray* stackFrames = vm::Array::New(il2cpp_defaults.stack_frame_class, len > skip ? len - skip : 0);

        bool use_native = len == pc_len;
        // IL2CPP_TRACE("[stacktrace] %d %d\n", len, pc_len);
        for (int i = skip; i < len; i++)
        {
            Il2CppStackFrame* stackFrame = NULL;

            if (use_native) 
            { 
                stackFrame = (Il2CppStackFrame*)vm::Object::New(il2cpp_defaults.stack_frame_class);
                //stackFrame = il2cpp_array_get(pc_stack_frames, Il2CppStackFrame*, i);
                MethodInfo* m = il2cpp_array_get(trace_ips, MethodInfo*, i);
                IL2CPP_OBJECT_SETREF(stackFrame, method, vm::Reflection::GetMethodObject(m, NULL));
                
                stackFrame->line = -1;
                Il2CppStackFrame* pcStackFrame = il2cpp_array_get(pc_stack_frames, Il2CppStackFrame*, i);
                if (pcStackFrame != NULL)
                {
                    stackFrame->il_offset = pcStackFrame->il_offset;
                    // mscorlib 的坑，filename不能为空字符串，需要长度大于0
                    if (pcStackFrame->filename != NULL && pcStackFrame->filename->length > 0)
                    {
                        IL2CPP_OBJECT_SETREF(stackFrame, filename, vm::String::NewUtf16(pcStackFrame->filename->chars, pcStackFrame->filename->length));
                    }
                }
            }
            else 
            {
                stackFrame = (Il2CppStackFrame*)vm::Object::New(il2cpp_defaults.stack_frame_class);
            }

            il2cpp_array_setref(stackFrames, i, stackFrame);
        }
        
        return stackFrames;
    }

    Il2CppArray* StackTrace::get_trace(Il2CppException *exc, int32_t skip, bool need_file_info)
    {
#if IL2CPP_ENABLE_STACKTRACE_PC_ADDRESS
        return GetTraceInternalWithLog(exc, skip, need_file_info);
#else
        // Exception.RestoreExceptionDispatchInfo() will clear trace_ips, so we need to ensure that we read it only once
        return GetTraceInternal(exc->trace_ips, skip, need_file_info);
#endif
    }
} /* namespace Diagnostics */
} /* namespace System */
} /* namespace mscorlib */
} /* namespace icalls */
} /* namespace il2cpp */
#endif
