cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# Build options are None, Debug, Release, RelWithDebInfo, MinSizeRel.
set(CMAKE_BUILD_TYPE Debug)

# Project name
# right now all code is in C, no CXX
project(A5ENGINE C)

# Logging options
option(USE_LOG "Use Logging" ON)

# Debugging options
option(USE_DEBUG "Use Debugging" ON)

# IDE
option(IDE "Build with an IDE")

# iPhone
option(IPHONE "Build for iPhone")

# Detect Linux
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(LINUX ON)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(LINUX OFF)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Enable Profiling on Debug Builds.
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
   set(USE_DEBUG ON)
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
if(USE_DEBUG)
 set(USE_PROFILE ON)
endif(USE_DEBUG)

# Definitions
if(USE_LOG)
    add_definitions(-DLOG)
endif(USE_LOG)

if(USE_DEBUG)
    add_definitions(-DVERBOSE)
    add_definitions(-DDEBUG)
   if(NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
   endif(NOT MSVC)
else(USE_DEBUG)
    add_definitions(-DNDEBUG)
   if(NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
   else(NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ox")
   endif(NOT MSVC)
endif(USE_DEBUG)

if(USE_PROFILE)
   add_definitions(-DPROFILE)
   if(NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
   endif(NOT MSVC)
endif(USE_PROFILE)

#Platform Specific Definitions
if(WIN32)
    add_definitions(-DWIN)
    if(MSVC)
        foreach(CFLAG
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        )
          if(${CFLAG} MATCHES "/MD")
              string(REGEX REPLACE "/MD" "/MT" ${CFLAG} "${${CFLAG}}")
          endif(${CFLAG} MATCHES "/MD")
        endforeach(CFLAG)
        set(CMAKE_EXTRA_LINKER_FLAGS "-SUBSYSTEM:WINDOWS")
    endif(MSVC)
    if(MINGW)
	set(CMAKE_EXTRA_LINKER_FLAGS "-mwindows -m32")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwindows -m32")
    endif(MINGW) 
endif(WIN32)

if(LINUX)
    add_definitions(-DLINUX)
endif(LINUX)

if(APPLE)
    set(WANT_X11 off)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
    if(IPHONE)
       set(CMAKE_EXTRA_LINKER_FLAGS "-framework CoreGraphics -framework QuartzCore -framework UIKit -framework Foundation -framework CoreFoundation -framework OpenGLES -framework OpenAL -lm")
       set(SDKROOT "iphoneos")
       set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
       set(CMAKE_OSX_SYSROOT "${SDKROOT}")
       add_definitions(-DIPHONE)
    else(IPHONE)
       set(CMAKE_EXTRA_LINKER_FLAGS "-framework Cocoa -framework QuartzCore -framework Foundation -framework CoreFoundation -framework OpenGL -framework IOKit -framework AudioToolbox -framework OpenAL -lm")
       add_definitions(-DAPPLE)
    endif(IPHONE)
endif(APPLE)

# find Module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules")

# add an include directory path
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Include")
include_directories("${PROJECT_INCLUDE_DIR}")

# look for Allegro library using CMake find module
find_package(Allegro REQUIRED)
include_directories(${ALLEGRO_INCLUDE_DIR})

# look for FreeType
set(ENV{FREETYPE_DIR} Deps)
find_package(Freetype REQUIRED)

# look for Lua library using CMake find module 
find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# look for Swig
find_package(SWIG REQUIRED)

# look for Chipmunk using CMake find module
find_package(Chipmunk REQUIRED)

# separate CMakeLists in Src/
set(subdir Src)

# build engine files as a library and generate Lua wrappers
add_subdirectory(${subdir})

# Bin/main
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(EXEC_FILES ${subdir}/driver.c ${CMAKE_CURRENT_BINARY_DIR}/${subdir}/gameLUA_wrap.c)

#Windows
if(WIN32)
   list(APPEND EXEC_FILES ${subdir}/main.rc)
endif(WIN32)

#iPhone
if(APPLE)
   if(IDE)
      add_executable(main MACOSX_BUNDLE ${EXEC_FILES})
      set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Data)
      set(APP_RES_DIR \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
      if(NOT IPHONE)
      set(APP_RES_DIR \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME}/Contents/Resources/)
      endif(NOT IPHONE)
      set(APP_DATA_DIR ${APP_RES_DIR}/Data)
      add_custom_command(
     	TARGET main
    	POST_BUILD
    	COMMAND mkdir -p ${APP_RES_DIR} && 
		ditto ${RES_DIR} ${APP_DATA_DIR} &&
		ditto ${RES_DIR}/icon.* ${APP_RES_DIR} &&
		echo "Copied Data to Bundle."
      )
      if(IPHONE)
         set_target_properties(main PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
         set_target_properties(main PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.ico)
      else(IPHONE)
         set_target_properties(main PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.icns)
      endif(IPHONE)
      set_target_properties(main PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "com.kiwi.Driver")
   else(IDE)
      add_executable(main ${EXEC_FILES})
   endif(IDE)
else(APPLE)
   add_executable(main ${EXEC_FILES})
endif(APPLE)

# Tell CMake the source won't be available until build time.
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${subdir}/gameLUA_wrap.c PROPERTIES GENERATED 1)
add_dependencies(main game_lua_wrapper)
# retrieve proper linkage from subdirectory via global property
get_property(ExtraLibraries GLOBAL PROPERTY ExtraLibraries)
target_link_libraries(main engine ${AllLibraries} ${ExtraLibraries} ${CMAKE_EXTRA_LINKER_FLAGS})

# optional CMake verbose: list variable values for testing
if(VERBOSE)
# ------------------------- Begin Generic CMake Variable Logging ------------------

# /*	C++ comment style not allowed	*/

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise 
# this is the top level directory of your build tree 
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this 
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to 
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

# this is the directory, from which cmake was started, i.e. the top level source directory 
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )

# this is the directory where the currently processed CMakeLists.txt is located in 
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )

# contains the full path to the top level directory of your build tree 
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command 
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

# set this variable to specify a common place where CMake should put all executable files
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )

# set this variable to specify a common place where CMake should put all libraries 
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )

# tell CMake to search first in directories listed in CMAKE_MODULE_PATH
# when you use FIND_PACKAGE() or INCLUDE()
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )

# this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake) 
MESSAGE( STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND} )

# this is the CMake installation directory 
MESSAGE( STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )

# this is the filename including the complete path of the file where this variable is used. 
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE} )

# this is linenumber where the variable is used
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE} )

# this is used when searching for include files e.g. using the FIND_PATH() command.
MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

# this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

# the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1" 
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )

# the short system name, e.g. "Linux", "FreeBSD" or "Windows"
MESSAGE( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )

# only the version part of CMAKE_SYSTEM 
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )

# the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz") 
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )

# is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
MESSAGE( STATUS "UNIX: " ${UNIX} )

#is TRUE on Linux
MESSAGE( STATUS "LINUX: " ${LINUX})

#is TRUE on iPhone
MESSAGE( STATUS "IPHONE: " ${IPHONE})

# is TRUE on Apple OS X
MESSAGE( STATUS "APPLE: " ${APPLE} )

# is TRUE on Windows, including CygWin
MESSAGE( STATUS "WIN32: " ${WIN32} )

# is TRUE when using the MinGW compiler in Windows
MESSAGE( STATUS "MINGW: " ${MINGW} )

# is TRUE on Windows when using the CygWin version of cmake
MESSAGE( STATUS "CYGWIN: " ${CYGWIN} )

# is TRUE on Windows when using a Borland compiler 
MESSAGE( STATUS "BORLAND: " ${BORLAND} )

# Microsoft compiler 
MESSAGE( STATUS "MSVC: " ${MSVC} )
MESSAGE( STATUS "MSVC_IDE: " ${MSVC_IDE} )
MESSAGE( STATUS "MSVC60: " ${MSVC60} )
MESSAGE( STATUS "MSVC70: " ${MSVC70} )
MESSAGE( STATUS "MSVC71: " ${MSVC71} )
MESSAGE( STATUS "MSVC80: " ${MSVC80} )
MESSAGE( STATUS "CMAKE_COMPILER_2005: " ${CMAKE_COMPILER_2005} )

# set this to true if you don't want to rebuild the object files if the rules have changed, 
# but not the actual source files or headers (e.g. if you changed the some compiler switches) 
MESSAGE( STATUS "CMAKE_SKIP_RULE_DEPENDENCY: " ${CMAKE_SKIP_RULE_DEPENDENCY} )

# since CMake 2.1 the install rule depends on all, i.e. everything will be built before installing. 
# If you don't like this, set this one to true.
MESSAGE( STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY} )

# If set, runtime paths are not added when using shared libraries. Default it is set to OFF
MESSAGE( STATUS "CMAKE_SKIP_RPATH: " ${CMAKE_SKIP_RPATH} )

# set this to true if you are using makefiles and want to see the full compile and link 
# commands instead of only the shortened ones 
MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )

# this will cause CMake to not put in the rules that re-run CMake. This might be useful if 
# you want to use the generated build files on another machine. 
MESSAGE( STATUS "CMAKE_SUPPRESS_REGENERATION: " ${CMAKE_SUPPRESS_REGENERATION} )


# A simple way to get switches to the compiler is to use ADD_DEFINITIONS(). 
# But there are also two variables exactly for this purpose: 

# the compiler flags for compiling C sources 
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )

# the compiler flags for compiling C++ sources 
MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )

# Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug) 
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

# if this is set to ON, then all libraries are built as shared libraries by default.
MESSAGE( STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )

# the compiler used for C files 
MESSAGE( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )

# the compiler used for C++ files 
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

# if the compiler is a variant of gcc, this should be set to 1 
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC} )

# if the compiler is a variant of g++, this should be set to 1 
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX} )

# the tools for creating libraries 
MESSAGE( STATUS "CMAKE_AR: " ${CMAKE_AR} )
MESSAGE( STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB} )

#print libraries
MESSAGE( STATUS "ALLEGRO_FOUND:               " ${ALLEGRO_FOUND} )
MESSAGE( STATUS "ALLEGRO_INCLUDE_DIR:         " ${ALLEGRO_INCLUDE_DIR} )
MESSAGE( STATUS "ALLEGRO_LIBRARIES:           " ${ALLEGRO_LIBRARIES} )
MESSAGE( STATUS "LUA_FOUND:                   " ${LUA_FOUND} )
MESSAGE( STATUS "LUA_INCLUDE_DIR:             " ${LUA_INCLUDE_DIR} )
MESSAGE( STATUS "LUA_LIBRARIES:               " ${LUA_LIBRARIES} )
MESSAGE( STATUS "CHIPMUNK_FOUND:              " ${CHIPMUNK_FOUND} )
MESSAGE( STATUS "CHIPMUNK_INCLUDE_DIR:        " ${CHIPMUNK_INCLUDE_DIR} )
MESSAGE( STATUS "CHIPMUNK_LIBRARIES:          " ${CHIPMUNK_LIBRARIES} )
MESSAGE( STATUS "FREETYPE_FOUND:              " ${FREETYPE_FOUND} )
MESSAGE( STATUS "FREETYPE_INCLUDE_DIRS:       " ${FREETYPE_INCLUDE_DIRS} )
MESSAGE( STATUS "FREETYPE_LIBRARIES:          " ${FREETYPE_LIBRARIES} )

#
#MESSAGE( STATUS ": " ${} )

# ------------------------- End of Generic CMake Variable Logging ------------------
endif(VERBOSE)
