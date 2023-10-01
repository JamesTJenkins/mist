#pragma once

#ifdef _WIN32
    #ifdef MIST_DLL
        #define MIST_API __declspec(dllexport)
    #else
        #define MIST_API __declspec(dllimport)
    #endif
#elif __linux__
    #define MIST_API
    #error "Linux not supported yet"
#else
    #error "Unsupported Platform"
#endif
