#ifndef CXX_LIBRARY_ADD__H
#define CXX_LIBRARY_ADD__H

#if defined(_WIN32)
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT /* not windows */
#endif

/* Add two integers. */
DLL_EXPORT int add(int a, int b);

#endif // CXX_LIBRARY_ADD__H
