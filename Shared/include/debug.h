#pragma once

#if _DEBUG

#include <cstdio>
#include <iostream>

#define DEBUG_SCOPE(x) do { x } while (0)
#define DEBUG_OPENCONSOLE() __consoleAttach()

#define DEBUG_PRINTF(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)
#define DEBUG_PRINT(x)  do { \
	std::cerr << x << std::endl; \
} while (0)
#define DEBUG_PRINT_E(x)  do { \
	fprintf(stderr, "<%s>: ", #x); \
	std::cerr << x << std::endl; \
} while (0)

#define DEBUG_HEXDUMP(x) __hexdump(&x, sizeof(x), nullptr);
#define DEBUG_HEXDUMP_E(x) __hexdump(&x, sizeof(x), #x);
#define DEBUG_HEXDUMP_N(x, n) __hexdump(&x, n, nullptr);
#define DEBUG_HEXDUMP_EN(x, n) __hexdump(&x, n, #x);

#define DEBUG_BINDUMP(x)  __bindump(&x, sizeof(x) * CHAR_BIT, nullptr);
#define DEBUG_BINDUMP_E(x) __bindump(&x, sizeof(x) * CHAR_BIT, #x);
#define DEBUG_BINDUMP_N(x, n) __bindump(&x, n, nullptr);
#define DEBUG_BINDUMP_EN(x, n) __bindump(&x, n, #x);

#define DEBUG_LAUNCH()   __debuggerLaunch();

bool __debuggerLaunch();
void __consoleAttach();
void __hexdump(const void* ptr, int buflen, const char* expr);
void __bindump(const void* ptr, int numbits, const char* expr);

#else

#define DEBUG_SCOPE(...)
#define DEBUG_OPENCONSOLE()

#define DEBUG_PRINTF(...)
#define DEBUG_PRINT(...) 
#define DEBUG_PRINT_E(...) 

#define DEBUG_HEXDUMP(...) 
#define DEBUG_HEXDUMP_E(...) 
#define DEBUG_HEXDUMP_N(...) 
#define DEBUG_HEXDUMP_EN(...) 

#define DEBUG_BINDUMP(...) 
#define DEBUG_BINDUMP_E(...) 
#define DEBUG_BINDUMP_N(...) 
#define DEBUG_BINDUMP_EN(...) 

#define DEBUG_LAUNCH()   

#endif