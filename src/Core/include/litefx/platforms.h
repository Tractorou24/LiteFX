#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // Target OS is Windows
    #define LITEFX_OS_WINDOWS 1
    #if defined(__clang__)
        // Compiler is Clang
        #define LITEFX_COMPILER_CLANG 1
    #elif defined(_MSC_VER)
        // Compiler is MSVC
        #define LITEFX_COMPILER_MSVC 1
    #else
        #error "Unknown or unsupported Windows compiler."
    #endif
#elif __APPLE__
    #error "Apple platform is not supported."
#elif defined(unix) | defined(__unix) | defined(__unix__)
    #ifdef __linux__
        // Target OS is Linux
        #define LITEFX_OS_LINUX 1
        #if defined(__clang__)
            // Compiler is Clang
            #define LITEFX_COMPILER_CLANG 1
        #elif defined(__GNUC__) || defined(__GNUG__)
            // Compiler is GCC
            #define LITEFX_COMPILER_GCC 1
        #else
            #error "Unknown or unsupported Linux compiler."
        #endif
    #else
        #error "Unknown or unsupported Unix platform."
    #endif
#else
    #error "Unknown and unsupported platform."
#endif