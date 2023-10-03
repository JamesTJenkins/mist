#pragma once

#ifdef MIST_DLL
    #define MIST_API __declspec(dllexport)
#else
    #define MIST_API __declspec(dllimport)
#endif

#if _WIN32
#elif __linux__
    #error "Linux not supported yet"
#else
    #error "Unsupported Platform"
#endif
