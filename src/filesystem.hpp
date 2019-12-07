#ifndef NGEN_FILESYSTEM__HPP
#define NGEN_FILESYSTEM__HPP

#if __cplusplus >= 201703L
#define HAVE_STD_FILESYSTEM 1
#elif defined(_MSC_VER) && ( _MSVC_LANG > 201402L && _MSC_VER >= 1915)
#define HAVE_STD_FILESYSTEM 1
#else
#define HAVE_STD_FILESYSTEM 0
#endif

#if HAVE_STD_FILESYSTEM
#include <filesystem>
#endif

#endif // NGEN_FILESYSTEM__HPP
