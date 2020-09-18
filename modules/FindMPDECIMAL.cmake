# Try to find the GNU Multiple Precision Arithmetic Library (GMP)
# See http://gmplib.org/
if (MPDECIMAL_INCLUDES AND MPDECIMAL_LIBRARIES)
	set(MPDECIMAL_FIND_QUIETLY TRUE)
endif ()
find_path(MPDECIMAL_INCLUDES
          NAMES
          mpdecimal.h
          PATHS
          $ENV{MPDECIMALDIR}
          ${INCLUDE_INSTALL_DIR}
          )
find_library(MPDECIMAL_LIBRARIES NAMES mpdecimal mpdec PATHS $ENV{MPDECIMALDIR} ${LIB_INSTALL_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPDECIMAL DEFAULT_MSG
                                  MPDECIMAL_INCLUDES MPDECIMAL_LIBRARIES)
mark_as_advanced(MPDECIMAL_INCLUDES MPDECIMAL_LIBRARIES)