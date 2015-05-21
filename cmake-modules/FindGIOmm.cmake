# - Try to find GIOmm 2.4
# Once done, this will define
#
#  GIOmm_FOUND - system has GIOmm
#  GIOmm_INCLUDE_DIR - the GIOmm include directories
#  GIOmm_LIBRARY - link these to use GIOmm

include(LibFindMacros)

# Dependencies
libfind_package(GIOmm GIO)
libfind_package(GIOmm Glibmm)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(GIOmm_PKGCONF giomm-2.4)

# Main include dir
find_path(GIOmm_INCLUDE_DIR
  NAMES giomm.h
  HINTS ${GIOmm_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES giomm-2.4
)

# Glib-related libraries also use a separate config header, which is in lib dir
find_path(GIOmmConfig_INCLUDE_DIR
  NAMES giommconfig.h
  HINTS ${GIOmm_PKGCONF_INCLUDE_DIRS} /usr
  PATH_SUFFIXES lib/giomm-2.4/include ../lib/giomm-2.4/include
)

# Finally the library itself
find_library(GIOmm_LIBRARY
  NAMES giomm-2.4
  HINTS ${GIOmm_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(GIOmm_PROCESS_INCLUDES GIOmm_INCLUDE_DIR GIOmmConfig_INCLUDE_DIR GIO_INCLUDE_DIR Glibmm_INCLUDE_DIR)
set(GIOmm_PROCESS_LIBS GIOmm_LIBRARY GIO_LIBRARY Glibmm_LIBRARY)
libfind_process(GIOmm)


