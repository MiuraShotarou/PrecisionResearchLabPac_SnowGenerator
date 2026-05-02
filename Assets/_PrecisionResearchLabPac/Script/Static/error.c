#include "error.h"

// プラットフォーム判定マクロ
#if defined(_WIN32) || defined(_WIN64)
    #define EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__linux__)
    #define EXPORT __attribute__((visibility("default")))
#else
    #define EXPORT
#endif

EXPORT const char* get_error_message(void)
{
    return _last_error_message;
}

