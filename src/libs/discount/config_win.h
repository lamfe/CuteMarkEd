#ifndef CONFIG_WIN_H
#define CONFIG_WIN_H

#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>

#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#define INITRNG(x) srand((unsigned int)x)
#define COINTOSS() (rand()&1)

#define VERSION "2.1.6"

#endif

#endif // CONFIG_H
