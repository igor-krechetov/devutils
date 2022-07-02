// Copyright (C) 2021 Igor Krechetov
// Distributed under MIT license. See file LICENSE for details

#ifndef LOGGINGUTILS_LOGGING_HPP
#define LOGGINGUTILS_LOGGING_HPP

#if defined(LOGGING_MODE_NICE)
#define USE_CONSOLE_ERRORS
#elif defined(LOGGING_MODE_OFF)
#define DISABLE_TRACES
#elif defined(LOGGING_MODE_DEBUG_OFF)
#define DISABLE_DEBUG_TRACES
#elif defined(LOGGING_MODE_STRICT_VERBOSE)
#define USE_CONSOLE_TRACES
#define EXIT_ON_FATAL
#else
#define USE_CONSOLE_ERRORS
#define EXIT_ON_FATAL
#endif

// ---------------------------------------------------------------------------------
// This macroses needs to de defined in each CPP files
// #undef TRACE_CLASS
// #define TRACE_CLASS                         "default"

#ifndef DISABLE_TRACES
 #ifndef PLATFORM_FREERTOS
  #include <sys/syscall.h>
  #include <unistd.h>
 #else
  #include <stdio.h>
 #endif

  // ---------------------------------------------------------------------------------
  //                     PRIVATE MACROSES
  // Don't use this in the code. It's for internal usage only
  extern int g_traces_pid;

 #ifndef PLATFORM_FREERTOS
  #define TRACE_CALL_COMMON()             const int _tid = syscall(__NR_gettid); (void)_tid
  #define TRACE_INIT()                    if (0 == g_traces_pid){ g_traces_pid = getpid(); }
 #else
  #define TRACE_CALL_COMMON()             const int _tid = 0; (void)_tid
  #define TRACE_INIT()
 #endif

  #define TRACE_CONSOLE_FORCE(msg, ...) \
      printf("[PID:%d, TID:%d] " TRACE_CLASS "::%s: " msg "\n", g_traces_pid, _tid, __func__,## __VA_ARGS__)
  
  #ifdef USE_CONSOLE_TRACES
    #define TRACE_CONSOLE(msg, ...)           TRACE_CONSOLE_FORCE(msg,## __VA_ARGS__)
    #define TRACE_ERROR_CONSOLE(msg, ...)
  #else
    #define TRACE_CONSOLE(msg, ...)
    #ifdef USE_CONSOLE_ERRORS
      #define TRACE_ERROR_CONSOLE(msg, ...)   TRACE_CONSOLE_FORCE(msg,## __VA_ARGS__)
    #else
      #define TRACE_ERROR_CONSOLE(msg, ...)
    #endif
  #endif

  #define TRACE_COMMON(msg, ...)          TRACE_CONSOLE(msg,## __VA_ARGS__)
  // ---------------------------------------------------------------------------------

  #define TRACE_PREINIT()                 int g_traces_pid = (0);
  #define TRACE(msg, ...)                 TRACE_COMMON(msg,## __VA_ARGS__)
  #define TRACE_WARNING(msg, ...)         TRACE_COMMON("[WARNING] " msg,## __VA_ARGS__)
  #define TRACE_ERROR(msg, ...)           TRACE_COMMON("[ERROR] " msg,## __VA_ARGS__); \
                                          TRACE_ERROR_CONSOLE("[ERROR] " msg,## __VA_ARGS__)
  #if defined(EXIT_ON_FATAL) && !defined(PLATFORM_FREERTOS)
    #define TRACE_FATAL(msg, ...)       TRACE_COMMON("[FATAL] " msg,## __VA_ARGS__); \
                                        TRACE_ERROR_CONSOLE("[FATAL] " msg,## __VA_ARGS__); \
                                        exit(1)
  #else
    #define TRACE_FATAL(msg, ...)       TRACE_COMMON("[FATAL] " msg,## __VA_ARGS__); \
                                        TRACE_ERROR_CONSOLE("[FATAL] " msg,## __VA_ARGS__)
  #endif  // EXIT_ON_FATAL
  #define TRACE_CALL()                    TRACE_CALL_COMMON(); \
                                          TRACE(" was called")
  #define TRACE_CALL_ARGS(msg, ...)       TRACE_CALL_COMMON(); \
                                          TRACE(msg,## __VA_ARGS__)
  #define TRACE_DEF()                     TRACE_CALL_COMMON()
#else
  #define TRACE_PREINIT()                 int g_traces_pid = (0);
  #define TRACE_INIT()
  #define TRACE(msg, ...)
  #define TRACE_WARNING(msg, ...)
  #define TRACE_ERROR(msg, ...)
  #define TRACE_FATAL(msg, ...)
  #define TRACE_CALL()
  #define TRACE_CALL_ARGS(msg, ...)
  #define TRACE_DEF()
#endif // DISABLE_TRACES

#if !defined(DISABLE_DEBUG_TRACES) && !defined(DISABLE_TRACES)
  #define TRACE_DEBUG(msg, ...) TRACE_COMMON("[DEBUG] " msg,## __VA_ARGS__)
  // Should be a first line in any function that needs traces
  #define TRACE_CALL_DEBUG()              TRACE_CALL_COMMON(); \
                                          TRACE_DEBUG(" was called")

  #define TRACE_CALL_DEBUG_ARGS(msg, ...) TRACE_CALL_COMMON(); \
                                          TRACE_DEBUG(msg,## __VA_ARGS__)

  #define TRACE_CALL_RESULT(msg, ...)     TRACE_DEBUG(" => " msg,## __VA_ARGS__)
  #define TRACE_LINE()                    TRACE_DEBUG("line:%d", __LINE__)
#else  // !defined(DISABLE_DEBUG_TRACES) && !defined(DISABLE_TRACES)
  #define TRACE_DEBUG(msg, ...)

  #ifndef DISABLE_TRACES
    #define TRACE_CALL_DEBUG()              TRACE_CALL_COMMON()
    #define TRACE_CALL_DEBUG_ARGS(msg, ...) TRACE_CALL_COMMON()
  #else
    #define TRACE_CALL_DEBUG()
    #define TRACE_CALL_DEBUG_ARGS(msg, ...)
  #endif

  #define TRACE_CALL_RESULT(msg, ...)
  #define TRACE_LINE()
#endif  // !defined(DISABLE_DEBUG_TRACES) && !defined(DISABLE_TRACES)

// ---------------------------------------------------------------------------------
//                          HELPERS
#ifndef SC2INT
  #define SC2INT(v)               static_cast<int>(v)
#endif
#ifndef BOOL2INT
  #define BOOL2INT(v) static_cast<int>(v)
#endif
#ifndef BOOL2STR
  #define BOOL2STR(v) ((v) ? "true" : "false")
#endif

#ifndef DEF2STR
  #define _DEF2STR_INTERNAL(s) #s
  #define DEF2STR(s) _DEF2STR_INTERNAL(s)
#endif

#endif  // LOGGINGUTILS_LOGGING_HPP
