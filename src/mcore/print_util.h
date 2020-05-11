#pragma once

#ifndef __print_util_h__
#define __print_util_h__

#if defined(_MSC_VER)
#include <Windows.h>
#endif
#include <stdio.h>
#include "mtime_ex.h"

#define print_level  6

#define print_level_err         1
#define print_level_warn        2
#define print_level_tag         3
#define print_level_info        4
#define print_level_debug       5
#define print_level_detail      6

#if 0
#define print_err(fmt,...)      if (print_level >= print_level_err)     printf("err:"    func_format_s fmt "\n", func_format(), __VA_ARGS__)
#define print_warn(fmt,...)     if (print_level >= print_level_warn)    printf("warn:"   func_format_s fmt "\n", func_format(), __VA_ARGS__)
#define print_tag(fmt,...)      if (print_level >= print_level_tag)     printf("tag:"    func_format_s fmt "\n", func_format(), __VA_ARGS__)
#define print_info(fmt,...)     if (print_level >= print_level_info)    printf("info:"   func_format_s fmt "\n", func_format(), __VA_ARGS__)
#define print_debug(fmt,...)    if (print_level >= print_level_debug)   printf("debug:"  func_format_s fmt "\n", func_format(), __VA_ARGS__)
#define print_detail(fmt,...)   if (print_level >= print_level_detail)  printf("detail:" func_format_s fmt "\n", func_format(), __VA_ARGS__)
#else

#if 0 // defined(_UNICODE )
#define print_err(fmt,...)      if (print_level >= print_level_err)     wprintf(L"[%s] err: "    func_format_s fmt L" %s:%d\n", mtime2sW(0), func_format(), __VA_ARGS__, TEXT(__FILE__), __LINE__)
#define print_warn(fmt,...)     if (print_level >= print_level_warn)    wprintf(L"[%s] warn: "   func_format_s fmt L" %s:%d\n", mtime2sW(0), func_format(), __VA_ARGS__, TEXT(__FILE__), __LINE__)
#define print_tag(fmt,...)      if (print_level >= print_level_tag)     wprintf(L"[%s] tag: "    func_format_s fmt L" %s:%d\n", mtime2sW(0), func_format(), __VA_ARGS__, __FILE__, __LINE__)
#define print_info(fmt,...)     if (print_level >= print_level_info)    wprintf(L"[%s] info: "   func_format_s fmt L" %s:%d\n", mtime2sW(0), func_format(), __VA_ARGS__, __FILE__, __LINE__)
#define print_debug(fmt,...)    if (print_level >= print_level_debug)   wprintf(L"[%s] debug: "  func_format_s fmt L" %s:%d\n", mtime2sW(0), func_format(), __VA_ARGS__, __FILE__, __LINE__)
#define print_detail(fmt,...)   if (print_level >= print_level_detail)  wprintf(L"[%s] detail: " func_format_s fmt L" %s:%d\n", mtime2sW(0), func_format(), __VA_ARGS__, __FILE__, __LINE__)
#else
#define print_err(fmt,...)      if (print_level >= print_level_err)     printf("[%s] err: "    func_format_s fmt " %s:%d\n", mtime2s(0), func_format(), ##__VA_ARGS__, __FILE__, __LINE__)
#define print_warn(fmt,...)     if (print_level >= print_level_warn)    printf("[%s] warn: "   func_format_s fmt " %s:%d\n", mtime2s(0), func_format(), ##__VA_ARGS__, __FILE__, __LINE__)
#define print_tag(fmt,...)      if (print_level >= print_level_tag)     printf("[%s] tag: "    func_format_s fmt " %s:%d\n", mtime2s(0), func_format(), ##__VA_ARGS__, __FILE__, __LINE__)
#define print_info(fmt,...)     if (print_level >= print_level_info)    printf("[%s] info: "   func_format_s fmt " %s:%d\n", mtime2s(0), func_format(), ##__VA_ARGS__, __FILE__, __LINE__)
#define print_debug(fmt,...)    if (print_level >= print_level_debug)   printf("[%s] debug: "  func_format_s fmt " %s:%d\n", mtime2s(0), func_format(), ##__VA_ARGS__, __FILE__, __LINE__)
#define print_detail(fmt,...)   if (print_level >= print_level_detail)  printf("[%s] detail: " func_format_s fmt " %s:%d\n", mtime2s(0), func_format(), ##__VA_ARGS__, __FILE__, __LINE__)
#endif // end of UNICODE

#endif

#endif
