#pragma once

#include <string>
#include <string_view>
#include <iterator>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <ranges>

#ifdef LITEFX_OS_WINDOWS
#define LITEFX_CODECVT_USE_WIN32
#include <Windows.h>
#else
#include <clocale>
#include <cwchar>
#include <cstring>
#endif

namespace LiteFX {
    
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    using String = std::string;
    using WString = std::wstring;
    using StringView = std::string_view;
    using WStringView = std::wstring_view;

    constexpr auto Join(std::ranges::input_range auto&& elements, StringView delimiter = ""sv) requires
        std::convertible_to<std::ranges::range_value_t<decltype(elements)>, String>
    {
        return std::ranges::fold_left(elements | std::views::join_with(delimiter), String{}, std::plus<>{});
    }

    constexpr auto WJoin(std::ranges::input_range auto&& elements, WStringView delimiter = L""sv) requires
        std::convertible_to<std::ranges::range_value_t<decltype(elements)>, String>
    {
        return std::ranges::fold_left(elements | std::views::join_with(delimiter), WString{}, std::plus<>{});
    }

    /// <summary>
    /// Computes the FNVa hash for <paramref name="string" />.
    /// </summary>
    /// <param name="string">The string to hash.</param>
    /// <returns>The FNVa hash for <paramref name="string" />.</returns>
    constexpr static std::uint64_t hash(StringView string) noexcept 
    {
        const std::uint64_t prime = 0x00000100000001b3;
        std::uint64_t seed = 0xcbf29ce484222325; // NOLINT
        
        for (auto ptr = string.begin(); ptr != string.end(); ptr++)
            seed = (seed ^ *ptr) * prime;

        return seed;
    }

    /// <summary>
    /// Computes the FNVa hash for <paramref name="string" />.
    /// </summary>
    /// <param name="string">The string to hash.</param>
    /// <returns>The FNVa hash for <paramref name="string" />.</returns>
    constexpr static std::uint64_t hash(WStringView string) noexcept 
    {
        const std::uint64_t prime = 0x00000100000001b3;
        std::uint64_t seed  = 0xcbf29ce484222325; // NOLINT

        for (auto ptr = string.begin(); ptr != string.end(); ptr++)
            seed = (seed ^ *ptr) * prime;

        return seed;
    }

    /// <summary>
    /// Computes the FNVa hash for <paramref name="string" />.
    /// </summary>
    /// <param name="string">The string to hash.</param>
    /// <param name="chars">The number of characters in the string.</param>
    /// <returns>The FNVa hash for <paramref name="string" />.</returns>
    consteval std::uint64_t operator ""_hash(const char* string, size_t chars) noexcept 
    {
        return hash(StringView(string, chars));
    }

    /// <summary>
    /// Computes the FNVa hash for <paramref name="string" />.
    /// </summary>
    /// <param name="string">The string to hash.</param>
    /// <param name="chars">The number of characters in the string.</param>
    /// <returns>The FNVa hash for <paramref name="string" />.</returns>
    consteval std::uint64_t operator ""_hash(const wchar_t* string, size_t chars) noexcept 
    {
        return hash(WStringView(string, chars));
    }

    /// <summary>
    /// Converts an UTF-8 single-byte encoded string into an wide multi-byte string (UTF-16 on Windows, UTF-32 on Linux).
    /// </summary>
    /// <param name="utf8"></param>
    /// <returns></returns>
    inline WString Widen(StringView utf8)
    {
#if defined LITEFX_CODECVT_USE_WIN32
        if (utf8.empty())
            return L"";

        const auto size = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);

        if (size <= 0)
            throw std::runtime_error("Unable to convert string to UTF-16: " + std::to_string(size));

        WString result(size, 0);
        ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), result.data(), size);

        return result;
#else
    // Ensure the system has a UTF-8 locale set.
    const char* original = std::setlocale(LC_CTYPE, nullptr);
    const bool modified = !original || std::strstr(original, "UTF-8") == nullptr;
    if (modified)
        std::setlocale(LC_CTYPE, "C.UTF-8");

    // Determine the size of the resulting wide string.
    mbstate_t state = {0};
    const char* src = utf8.data();
    const auto size = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (size == static_cast<std::size_t>(-1))
        throw std::runtime_error("Unable to convert string to UTF-16.");

    // Convert the string.
    WString result(size, 0);
    state = {0};
    src = utf8.data();
    std::mbsrtowcs(result.data(), &src, size + 1, &state);

    // Restore the original locale if modified.
    if(modified && original)
        std::setlocale(LC_CTYPE, original);
    return result;
#endif
    }

    /// <summary>
    /// Converts a wide multi-byte encoded string (UTF-16 on Windows, UTF-32 on Linux) into an UTF-8 representation.
    /// </summary>
    /// <param name="wide"></param>
    /// <returns></returns>
    inline String Narrow(WStringView wide)
    {
#if defined LITEFX_CODECVT_USE_WIN32
        if (wide.empty())
            return "";

        const auto size = ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);

        if (size <= 0)
            throw std::runtime_error("Unable to convert string to UTF-8: " + std::to_string(size));

        String result(size, 0);
        ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), result.data(), size, nullptr, nullptr);

        return result;
#else
        // Ensure the system has a UTF-8 locale set.
        const char* original = std::setlocale(LC_CTYPE, nullptr);
        const bool modified = !original || std::strstr(original, "UTF-8") == nullptr;
        if (modified)
            std::setlocale(LC_CTYPE, "C.UTF-8");

        // Determine the size of the resulting narrow string.
        mbstate_t state = {0};
        const wchar_t* src = wide.data();
        const auto size = std::wcsrtombs(nullptr, &src, 0, &state);
        if (size == static_cast<std::size_t>(-1))
            throw std::runtime_error("Unable to convert string to UTF-8.");

        // Convert the string.
        String result(size, 0);
        state = {0};
        src = wide.data();
        std::wcsrtombs(result.data(), &src, size + 1, &state);

        // Restore the original locale if modified.
        if(modified && original)
            std::setlocale(LC_CTYPE, original);
        return result;
#endif
    }
}

#undef LITEFX_CODECVT_USE_WIN32