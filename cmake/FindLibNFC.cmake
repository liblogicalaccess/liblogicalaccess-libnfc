# - Find LibNFC
# Find LibNFC includes and library
#
#  LIBNFC_INCLUDE_DIRS - where to find nfc.h, nfc-types.h, etc.
#  LIBNFC_LIBRARYDIR - where to find libnfc.lib, nfc.a, etc.
#  LIBNFC_LIBRARIES   - List of libraries when using LibNFC.
#  LIBNFC_FOUND       - True if LibNFC found.


IF (LIBNFC_INCLUDE_DIRS AND LIBNFC_LIBRARIES)
  # Already in cache, be silent
  SET(LIBNFC_FIND_QUIETLY TRUE)
ENDIF (LIBNFC_INCLUDE_DIRS AND LIBNFC_LIBRARIES)

IF (NOT WIN32)
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(PC_LIBNFC libnfc)
ENDIF (NOT WIN32)

FIND_PATH(LIBNFC_INCLUDE_DIRS nfc/nfc-types.h
  HINTS
  ${PC_LIBNFC_INCLUDEDIR}
  ${PC_LIBNFC_INCLUDE_DIRS}
  ${PC_LIBNFC_INCLUDE_DIRS}/LIBNFC
  )

set (LIBNFC ${LIBNFC_INCLUDE_DIRS})
FIND_LIBRARY(LIBNFC_LIBRARY NAMES libnfc nfc
  HINTS
  ${LIBNFC_LIBRARYDIR}
  ${PC_LIBNFC_LIBDIR}
  ${PC_LIBNFC_LIBRARY_DIRS}
  )

message(INFO ${LIBNFC_LIBRARY})

SET( LIBNFC_LIBRARIES ${LIBNFC_LIBRARY} )

# handle the QUIETLY and REQUIRED arguments and set LIBNFC_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibNFC DEFAULT_MSG LIBNFC_LIBRARY LIBNFC_INCLUDE_DIRS)

MARK_AS_ADVANCED( LIBNFC_LIBRARY LIBNFC_INCLUDE_DIRS )

