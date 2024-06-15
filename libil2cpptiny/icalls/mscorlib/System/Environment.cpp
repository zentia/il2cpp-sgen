#include "il2cpp-config.h"

#include "il2cpp-object-internals.h"
#include "il2cpp-class-internals.h"
#include "il2cpp-string-types.h"
#include "Environment.h"
#include "os/StackTrace.h"
#include "utils/StringUtils.h"
#include "utils/Environment.h"
#include "vm/Array.h"
#include "vm/Runtime.h"
#include "vm/String.h"
#include "vm/StackTrace.h"

#include <string>
#include <vector>

extern TinyType* g_StringTinyType;

namespace tiny
{
namespace icalls
{
namespace mscorlib
{
namespace System
{
    Il2CppString* Environment::GetStackTrace_internal()
    {
        std::string stackTrace = tiny::vm::StackTrace::GetStackTrace();
        UTF16String utf16Chars = il2cpp::utils::StringUtils::Utf8ToUtf16(stackTrace.c_str(), stackTrace.length());
        return vm::String::NewLen(utf16Chars.c_str(), (uint32_t)stackTrace.length());
    }

    void Environment::FailFast_internal(Il2CppString* message)
    {
        std::string messageUtf8;
        if (message != NULL)
            messageUtf8 = il2cpp::utils::StringUtils::Utf16ToUtf8(message->chars, message->length);

        vm::Runtime::FailFast(messageUtf8.c_str());
    }

    Il2CppArray* Environment::GetCommandLineArgs()
    {
        Il2CppArray *res;
        int i;
        int num_main_args = il2cpp::utils::Environment::GetNumMainArgs();
        const std::vector<UTF16String>& mainArgs = il2cpp::utils::Environment::GetMainArgs();

        res = vm::Array::New<Il2CppString*>(g_StringTinyType, sizeof(Il2CppString*), num_main_args);

        for (i = 0; i < num_main_args; ++i)
            il2cpp_array_setref(res, i, vm::String::NewLen(mainArgs[i].c_str(), static_cast<int>(mainArgs[i].length())));

        return res;
    }
} /* namespace System */
} /* namespace mscorlib */
} /* namespace icalls */
} /* namespace tiny */
