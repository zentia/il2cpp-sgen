#include "il2cpp-config.h"
#include "il2cpp-object-internals.h"
#include "Exception.h"
#include "os/CrashHelpers.h"
#include "utils/StringUtils.h"
#include "utils/Logging.h"
#include "vm/StackTrace.h"

#include <string>

namespace tiny
{
namespace vm
{
    void Exception::RaiseInvalidCastException(Il2CppObject* obj, TinyType* tinyType)
    {
        il2cpp::os::CrashHelpers::Crash();
    }

    void Exception::RaiseGetIndexOutOfRangeException()
    {
        il2cpp::os::CrashHelpers::Crash();
    }

    void Exception::Raise()
    {
        Raise((const char*)NULL);
    }

    void Exception::Raise(Il2CppException* exception)
    {
        if (exception != NULL && exception->message != NULL)
            Raise(il2cpp::utils::StringUtils::Utf16ToUtf8(exception->message->chars, exception->message->length).c_str());
        else
            Raise((const char*)NULL);
    }

    void Exception::Raise(const char* exceptionMessage)
    {
        il2cpp::utils::Logging::Write("A managed exception was thrown. The Tiny runtime does not support managed exceptions.");
        if (exceptionMessage != NULL)
        {
            std::string message = "The exception message is: ";
            message += exceptionMessage;
            il2cpp::utils::Logging::Write(message.c_str());
        }
        else
        {
            il2cpp::utils::Logging::Write("No message was provided to the managed exception.");
        }

        StackTrace::LogManagedStackTrace();

        il2cpp::os::CrashHelpers::Crash();
    }
}
}
