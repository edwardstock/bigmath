if (BIGMATH_INCLUDES AND BIGMATH_LIBRARIES)
	set(BIGMATH_FIND_QUIETLY TRUE)
endif (BIGMATH_INCLUDES AND BIGMATH_LIBRARIES)
find_path(BIGMATH_INCLUDES
          NAMES
          bigmath/bigmath_config.h
          bigmath/bigint.h
          bigmath/bigdecimal.h
          PATHS
          $ENV{BIGMATHDIR}
          ${INCLUDE_INSTALL_DIR}
          )
find_library(BIGMATH_LIBRARIES bigmath PATHS $ENV{BIGMATHDIR} ${BIGMATH_ROOT} ${LIB_INSTALL_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BigMath DEFAULT_MSG
                                  BIGMATH_INCLUDES BIGMATH_LIBRARIES)
mark_as_advanced(BIGMATH_INCLUDES BIGMATH_LIBRARIES)