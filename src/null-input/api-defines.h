#pragma once

#if defined(NULLINPUT_STATIC_LINKING)
    #define NULLINPUT_API
#elif defined(NULLINPUT_DYNAMIC_LINKING)
    #ifdef NULLINPUT_EXPORTING
        #define NULLINPUT_API __declspec(dllexport)
    #else
        #define NULLINPUT_API __declspec(dllimport)
    #endif
#else
    #error unknown null-input linking type
#endif