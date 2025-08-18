#pragma once

/* 可視性マクロ */
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(FOXCLIP_STATIC)
#define FOXCLIP_API
#else
#if defined(FOXCLIP_BUILDING)
#define FOXCLIP_API __declspec(dllexport)
#else
#define FOXCLIP_API __declspec(dllimport)
#endif
#endif
#define FOXCLIP_CALL __cdecl
#else
#define FOXCLIP_API  __attribute__((visibility("default")))
#define FOXCLIP_CALL
#endif

/* C/C++ 両対応 */
#ifdef __cplusplus
#define FOXCLIP_EXTERN extern "C"
#else
#define FOXCLIP_EXTERN extern
#endif
