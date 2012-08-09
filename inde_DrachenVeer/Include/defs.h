#ifndef _DEFS_H_
#define _DEFS_H_

//Header file which does all the macros and configuration stuff.

#ifdef WINVER
#define WIN
#endif //WINVER

#ifdef _MSC_VER
#define MSVC
#endif //_MSC_VER

#ifdef __linux__
#ifndef LINUX
#define LINUX
#endif //LINUX
#endif //__linux__

//String Compare without Case
#ifdef MSVC
#if _MSC_VER >= 1600
#define strcasecmp _stricmp
#define _CRT_SECURE_NO_WARNINGS
#else
#define strcasecmp stricmp
#define _CRT_SECURE_NO_DEPRECATE
#endif //_MSC_VER >= 1600
#define strncasecmp _strnicmp
#endif //MSVC

//Inlining Code.
#ifdef MSVC
#ifndef CINLINE
#define CINLINE 
#endif //CINLINE
#else
#define CINLINE inline
#endif //CINLINE

//Path Seperator Characters for Directory seperators in paths
#ifdef WIN
#define PATHSEP '\\'
#define SYSSEP  ';'
#define LINESEP "\r\n"
#else
#define PATHSEP '/'
#define SYSSEP  ':'
#define LINESEP "\n"
#endif

//Platform Specific IDs.
#if defined(WIN)
#define PLAT_ID "win"
#elif defined(IPHONE)
#define PLAT_ID "iphone"
#elif defined(APPLE)
#define PLAT_ID "mac"
#elif defined(LINUX)
#define PLAT_ID "linux"
#endif //defined

//Useful buffer size for strings
#define MAXSTR 256

//Useful large buffer size for strings
#define MAXSTRLARGE 4096

//Loads 1-2 Levels at a time
#define LOAD_LEVEL_DEFERRED

//Enables Game Log -- Use CMake option
//#define LOG
//#define VERBOSE

//Enables Debugging Code -- Use CMake option
//#define DEBUG

#endif //_DEFS_H_
