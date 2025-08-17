#pragma once

/* 可視性マクロ */
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(FOXCLIP_STATIC)
#define FC_API
#else
#if defined(FOXCLIP_BUILDING)
#define FC_API __declspec(dllexport)
#else
#define FC_API __declspec(dllimport)
#endif
#endif
#define FC_CALL __cdecl
#else
#define FC_API __attribute__((visibility("default")))
#define FC_CALL
#endif

/* C/C++ 両対応 */
#ifdef __cplusplus
#define FC_EXTERN extern "C"
#else
#define FC_EXTERN extern
#endif
