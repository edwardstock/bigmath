include(CheckIncludeFileCXX)
include(CheckTypeSize)


check_type_size("unsigned __int128" HAVE_UINT128_T LANGUAGE CXX)
check_type_size("unsigned long long" HAVE_UINT64_T LANGUAGE CXX)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/bigmath_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/bigmath/bigmath_config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/bigmath.pc.in ${CMAKE_BINARY_DIR}/pkgconfig/bigmath.pc @ONLY)