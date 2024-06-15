#pragma once

#include <string>
#include "Baselib_Utilities.h"
#include "Include/External/utfcpp/source/utf8/unchecked.h"

namespace detail
{
    static inline std::string StrUtf16ToUtf8(const baselib_char16_t* utf16String, const size_t length)
    {
        std::string utf8String;
        utf8String.reserve(length);
        utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(utf8String));
        return utf8String;
    }

    static inline std::string StrUtf16ToUtf8(const baselib_char16_t* utf16String)
    {
        return StrUtf16ToUtf8(utf16String, StrLenUtf16(utf16String));
    }

    static inline std::string StrUtf16ToUtf8(const std::basic_string<baselib_char16_t>& utf16String)
    {
        return StrUtf16ToUtf8(utf16String.c_str(), utf16String.length());
    }

    static inline std::basic_string<baselib_char16_t> StrUtf8ToUtf16(const char* utf8String, const size_t length)
    {
        std::basic_string<baselib_char16_t> utf16String;
        if (utf8::is_valid(utf8String, utf8String + length))
        {
            utf16String.reserve(length);
            utf8::unchecked::utf8to16(utf8String, utf8String + length, std::back_inserter(utf16String));
        }

        return utf16String;
    }

    static inline std::basic_string<baselib_char16_t> StrUtf8ToUtf16(const char* utf8String)
    {
        return StrUtf8ToUtf16(utf8String, strlen(utf8String));
    }

    static inline std::basic_string<baselib_char16_t> StrUtf8ToUtf16(const std::string& utf8String)
    {
        return StrUtf8ToUtf16(utf8String.c_str(), utf8String.length());
    }
}
