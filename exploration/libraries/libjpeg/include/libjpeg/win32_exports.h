#if !defined(__WIN32_EXPORTS__H__)
#define __WIN32_EXPORTS__H__

#if defined(WIN32)
#if defined(JPEG_STATIC) || defined(_LIB)
#define JPEG_API
#else
#if defined(JPEG_EXPORTS)
#define JPEG_API __declspec(dllexport)
#else
#define JPEG_API __declspec(dllimport)
#endif
#endif
#else
#define JPEG_API
#endif
#endif  //__WIN32_EXPORTS__H__
