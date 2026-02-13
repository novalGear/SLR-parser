#pragma once

#include <cstdio>

#ifdef DEBUG
    #define DBG_PRINT(...) \
        do { \
            std::fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            std::fprintf(stderr, __VA_ARGS__); \
            std::fputc('\n', stderr); \
        } while(0)
#else
    #define DBG_PRINT(...) do { } while(0)
#endif
