cmake_minimum_required(VERSION 3.9)
project(bigmath VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
if (WIN32)
	execute_process(COMMAND cmd /C "echo ${CMAKE_PROJECT_VERSION} > ${CMAKE_CURRENT_SOURCE_DIR}/version")
else ()
	execute_process(COMMAND bash -c "echo ${CMAKE_PROJECT_VERSION} > ${CMAKE_CURRENT_SOURCE_DIR}/version")
endif ()


include(modules/compilers_flags.cmake)

option(ENABLE_CONAN "Enable dependency manager" On)
option(ENABLE_SHARED "Build shared library" Off)
option(ENABLE_TEST "Build test target" Off)

set(BIGMATHPP_EXPORTING 1)
if (ENABLE_SHARED)
	set(BIGMATHPP_SHARED 1)
endif ()


set(HEADERS
    include/bigmath/bigint.h
    include/bigmath/mpalloc.h
    include/bigmath/mpdecimal_backport.h
    include/bigmath/typearith.h
    include/bigmath/bigmath_config.h
    include/bigmath/errors.h
    include/bigmath/utils.h include/bigmath/bd_context.h)

set(SOURCES
    ${HEADERS}
    src/bigint.cpp
    src/mpalloc.cpp
    src/mpdecimal_backport.cpp
    src/bigdecimal.cpp
    src/bd_context.cpp)

if (ENABLE_SHARED)
	message(STATUS "Build shared library")
	add_library(${PROJECT_NAME} SHARED ${SOURCES})
	set_property(TARGET ${PROJECT_NAME} PROPERTY
	             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else ()
	add_library(${PROJECT_NAME} STATIC ${SOURCES})
	set_property(TARGET ${PROJECT_NAME} PROPERTY
	             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()


target_include_directories(${PROJECT_NAME} PUBLIC include)

if (ENABLE_CONAN)
	include(ConanInit)
	conan_init()

	set(HAVE_MPIR 1)
	#	target_link_libraries(${PROJECT_NAME} CONAN_PKG::gmp)
	target_link_libraries(${PROJECT_NAME} CONAN_PKG::mpir)
	target_link_libraries(${PROJECT_NAME} CONAN_PKG::mpdecimal)
else ()
	find_package(GMP 6.2.0)
	find_package(MPIR 3.0.0)

	if (GMP_LIBRARIES-NOTFOUND AND MPIR_LIBRARIES-NOTFOUND)
		message(FATAL_ERROR "GMP or MPIR library must be installed to system or set prefix -DGMP_ROOT=/path/to/gmp or -DMPIR_ROOT=/path/to/mpir")
	endif ()

	if (GMP_LIBRARIES)
		set(_GMP_COMPAT_LIBS ${GMP_LIBRARIES})
		set(_GMP_COMPAT_INC ${GMP_INCLUDES})
		set(HAVE_GMP 1)
	elseif (MPIR_LIBRARIES)
		set(_GMP_COMPAT_LIBS ${MPIR_LIBRARIES})
		set(_GMP_COMPAT_INC ${MPIR_INCLUDES})
		set(HAVE_MPIR 1)
	endif ()

	find_package(MPDECIMAL 2.4.2 REQUIRED)

	target_link_libraries(${PROJECT_NAME} ${_GMP_COMPAT_LIBS})
	target_link_libraries(${PROJECT_NAME} ${MPDECIMAL_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PUBLIC ${_GMP_COMPAT_INC})
	target_include_directories(${PROJECT_NAME} PUBLIC ${MPDECIMAL_INCLUDES})
endif ()

include(SystemDetector)

if (ENABLE_TEST)

	set(gtest_force_shared_crt ${ENABLE_SHARED} CACHE BOOL "" FORCE)

	if (APPLE)
		add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
		add_definitions(-D__GLIBCXX__)
	endif ()

	enable_testing()
	add_definitions(-DBIGMATH_TESTING=1)

	add_executable(${PROJECT_NAME}-test
	               tests/main.cpp
	               tests/bigint_test.cpp
	               tests/bigdecimal_test.cpp)

	if (ENABLE_PVS)
		include(PVS-Studio)
		pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
		                      OUTPUT FORMAT errorfile
		                      CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/pvs.cfg
		                      ANALYZE ${PROJECT_NAME}-test
		                      SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/include
		                      LOG target.err)
	endif ()

	if (NOT MSVC)
		target_compile_options(${PROJECT_NAME}-test PRIVATE -Wno-missing-field-initializers)
	endif ()

	if (NOT ENABLE_CONAN)
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/gtest)
		target_link_libraries(${PROJECT_NAME}-test gtest)
	else ()
		target_link_libraries(${PROJECT_NAME}-test CONAN_PKG::gtest)
	endif ()

	target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})

	if (WITH_COVERAGE)
		set(COVERAGE_LCOV_EXCLUDES
		    'libs/*'
		    '/usr/include/*'
		    'tests/*'
		    'boost/*'
		    '/usr/include/*'
		    '/usr/local/*'
		    'v1'
		    '4.9'
		    )
		include(modules/CodeCoverage.cmake)
		append_coverage_compiler_flags()
		setup_target_for_coverage_lcov(
			NAME ${PROJECT_NAME}-coverage
			EXECUTABLE ${PROJECT_NAME}-test
			DEPENDENCIES ${PROJECT_NAME}-test
		)
	endif ()

	add_test(NAME test-all COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-test)

endif ()

install(
	TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/bigmath
	DESTINATION include
)