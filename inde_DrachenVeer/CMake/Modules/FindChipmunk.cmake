# - Try to find CHIPMUNK
# Once done, this will define
# CHIPMUNK_INCLUDE_DIR - chipmunk.h etc.
# CHIPMUNK_LIBRARIES - for linking
# CHIPMUNK_FOUND - True if found

set(CHIPMUNK_VERSION_SUFFIX "")

find_path(CHIPMUNK_INCLUDE_DIR chipmunk/chipmunk.h
	HINTS "${PROJECT_SOURCE_DIR}/Deps/include" "$ENV{CHIPMUNK_HOME}/include" "$ENV{EXTDEV}/include" "$ENV{EPREFIX}/include" "$ENV{PREFIX}/include" "/usr/include" "/usr/include/chipmunk${CHIPMUNK_VERSION_SUFFIX}" "/usr/local/include" "/usr/local/include/${CHIPMUNK_VERSION_SUFFIX}"
	)

find_library(CHIPMUNK_LIBRARY
	NAMES "chipmunk${CHIPMUNK_VERSION_SUFFIX}" "chipmunk"
	PATHS "${PROJECT_SOURCE_DIR}/Deps/lib" "${PROJECT_SOURCE_DIR}/Deps/lib/MinGW"  "${PROJECT_SOURCE_DIR}/Deps/lib/MSVC" "$ENV{EXTDEV}/lib" "$ENV{EXTDEV}/lib/MinGW" "$ENV{EXTDEV}/lib/MSVC" "$ENV{CHIPMUNK_HOME}/lib" "$ENV{EPREFIX}/lib" "$ENV{PREFIX}/lib" "/usr/lib" "/usr/local/lib")

include(FindPackageHandleStandardArgs)
# handle QUIETLY and REQUIRED args and set CHIPMUNK_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(Chipmunk DEFAULT_MSG
				CHIPMUNK_LIBRARY CHIPMUNK_INCLUDE_DIR)
mark_as_advanced(CHIPMUNK_INCLUDE_DIR CHIPMUNK_LIBRARY)
