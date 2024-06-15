#pragma once

struct Il2CppException;
struct Il2CppObject;
struct TinyType;

namespace tiny
{
namespace vm
{
    class Exception
    {
    public:
        NORETURN static void RaiseInvalidCastException(Il2CppObject* obj, TinyType* tinyType);
        NORETURN static void RaiseGetIndexOutOfRangeException();
        NORETURN static void Raise();
        NORETURN static void Raise(Il2CppException* exception);
        NORETURN static void Raise(const char* exceptionMessage);
    };
}
}
