#Default Options.

set(AllLibraries ${AllLibraries})

#Base Files.
list(APPEND EngineFiles list.c hash.c pool.c log.c state.c usual.c util.c transfx.c shader.c
                        draw.c fonts.c camera.c emu3d.c log.c global.c
                        object.c objload.c objrender.c levanim.c levload.c levrender.c spranim.c sprload.c sprrender.c
                        gameglobal.c gameplay.c script.c actions.c splash.c)

#Physics.
list(APPEND EngineFiles physicschipmunk.c)
include_directories(${CHIPMUNK_INCLUDE_DIR})

#GUI
option(USE_INTERNAL_LIB "Use the WidgetZ library internally" ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/Src/WidgetZ)
include_directories(${WIDGETZ_SOURCE_DIR}/include)

list(APPEND EngineFiles gui.c)

#functions to expose to Lua script
list(APPEND LuaHeaders appinfo.h object.h log.h sprite.h gui.h actions.h gameglobal.h)

#Create Swig interface file including each header
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")
set(INTERFACE_PREFIX ${PROJECT_SOURCE_DIR}/Include)
set(SWIG_I_DIR ${CMAKE_CURRENT_BINARY_DIR}/swig)
set(SWIG_I_FILE "game.i")
include_directories(${SWIG_I_DIR})
set(WRAPPER_SRC "")
set(SWIG_I_MODULE "%module game\n%{\n")
set(SWIG_I_INCLUDE "")
set(SWIG_H_FILES "")
foreach(itemf ${LuaHeaders})
	#filename without extension
	get_filename_component(item ${itemf} NAME_WE)
	set(SWIG_I_MODULE "${SWIG_I_MODULE}#include \"${item}.h\"\n")
	set(SWIG_I_INCLUDE "${SWIG_I_INCLUDE}%include \"${INTERFACE_PREFIX}/${item}.h\"\n")
	list(APPEND SWIG_H_FILES "${INTERFACE_PREFIX}/${item}.h")
endforeach(itemf ${LuaHeaders})

#Create Lua wrappers by invoking SWIG
swig_module_initialize(game lua)
set(SWIG_I_MODULE "${SWIG_I_MODULE}%}\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_I_FILE} "${SWIG_I_MODULE} ${SWIG_I_INCLUDE}")
swig_add_source_to_module(game swig_generated_source ${CMAKE_CURRENT_BINARY_DIR}/${SWIG_I_FILE} ${SWIG_H_FILES})

#Create a dependency rule for generated source for use in parent-scope target
add_custom_target(game_lua_wrapper DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gameLUA_wrap.c)

# Compile all sources into a library
add_library(engine ${EngineFiles})

# Link the Allegro and Lua Libraries
list(APPEND AllLibraries ${ALLEGRO_LIBRARIES} ${LUA_LIBRARY})

# Link the GUI library
option(USE_INTERNAL_LIB "Use the WidgetZ library internally" ON)
list(APPEND AllLibraries widgetz)

# Link the Chipmunk library
list(APPEND AllLibraries ${CHIPMUNK_LIBRARY})

# Link the Freetype Library
list(APPEND AllLibraries ${FREETYPE_LIBRARIES})

# set a global property (not variable) to the new list of libraries so it can be retrieved in parent scope
set_property(GLOBAL APPEND PROPERTY ExtraLibraries "${AllLibraries}")

# The following helped when understanding how to add generated source files to CMake:
# http://www.cmake.org/Wiki/CMake_FAQ#How_can_I_add_a_dependency_to_a_source_file_which_is_generated_in_a_subdirectory.3F
