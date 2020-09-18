# Try to find the GNU Multiple Precision Arithmetic Library (GMP)
# See http://gmplib.org/
if (MPIR_INCLUDES AND MPIR_LIBRARIES)
	set(MPIR_FIND_QUIETLY TRUE)
endif ()

if (MPIR_ROOT)
	set(_MPIR_ROOT_INCLUDE ${MPIR_ROOT}/include)
	set(_MPIR_ROOT_LIB ${MPIR_ROOT}/lib)
endif ()


find_path(MPIR_INCLUDES
          NAMES
          mpir.h
          mpirxx.h
          PATHS
          $ENV{MPIRDIR}
          ${INCLUDE_INSTALL_DIR}
          ${_MPIR_ROOT_INCLUDE}
          )
find_library(MPIR_LIBRARIES NAMES mpir mpirxx PATHS $ENV{MPIRDIR} ${_MPIR_ROOT_LIB} ${LIB_INSTALL_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPIR DEFAULT_MSG
                                  MPIR_INCLUDES MPIR_LIBRARIES)
mark_as_advanced(MPIR_INCLUDES MPIR_LIBRARIES)