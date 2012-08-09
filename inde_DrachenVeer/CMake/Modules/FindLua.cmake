# - Try to find Lua
# Once done, this will define
# LUA_INCLUDE_DIR - lua.h etc.
# LUA_LIBRARIES - for linking
# LUA_FOUND - True if found

set(LUA_VERSION_SUFFIX "5.1")

find_path(LUA_INCLUDE_DIR lua.h
	HINTS "${PROJECT_SOURCE_DIR}/Deps/include" "$ENV{LUA_HOME}/include" "$ENV{EXTDEV}/include" "$ENV{EPREFIX}/include" "$ENV{PREFIX}/include" "/usr/include" "/usr/include/lua${LUA_VERSION_SUFFIX}" "/usr/local/include" "/usr/local/include/${LUA_VERSION_SUFFIX}"
	)

find_library(LUA_LIBRARY
	NAMES "lua${LUA_VERSION_SUFFIX}" "lua"
	PATHS "${PROJECT_SOURCE_DIR}/Deps/lib" "${PROJECT_SOURCE_DIR}/Deps/lib/MinGW"  "${PROJECT_SOURCE_DIR}/Deps/lib/MSVC" "$ENV{EXTDEV}/lib" "$ENV{EXTDEV}/lib/MinGW" "$ENV{EXTDEV}/lib/MSVC" "$ENV{LUA_HOME}/lib" "$ENV{EPREFIX}/lib" "$ENV{PREFIX}/lib" "/usr/lib" "/usr/local/lib"
   )

include(FindPackageHandleStandardArgs)
# handle QUIETLY and REQUIRED args and set LUA_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(Lua DEFAULT_MSG
				LUA_LIBRARY LUA_INCLUDE_DIR)
mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARY)
