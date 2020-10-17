include(FeatureSummary)
include(FindLinuxPlatform)
include(CMakePackageConfigHelpers)

list(APPEND INSTALL_TARGETS ${PROJECT_NAME})
set(INSTALL_BIN_DIR bin)
set(INSTALL_LIB_DIR lib/${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
set(INSTALL_CMAKE_DIR ${INSTALL_LIB_DIR}/cmake)
set(INSTALL_INCLUDE_DIR include)

if (HAVE_GMP)
	list(APPEND target_deps "find_dependency(GMP REQUIRED)")
elseif (HAVE_MPIR)
	list(APPEND target_deps "find_dependency(MPIR REQUIRED)")
endif ()
list(APPEND target_deps "find_dependency(MPDECIMAL REQUIRED)")

if (target_deps)
	list(JOIN target_deps "\n" targets_deps_joined)
	set(HAS_TARGET_DEPS 1)
endif ()


configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cfg/${PROJECT_NAME}-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	INSTALL_DESTINATION lib/${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}/cmake
	PATH_VARS INSTALL_LIB_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

list(JOIN INSTALL_TARGETS " -l" INSTALL_TARGETS_LINK_FLAG)
set(INSTALL_TARGETS_LINK_FLAG "-l${INSTALL_TARGETS_LINK_FLAG}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/bigmath.pc.in ${CMAKE_BINARY_DIR}/pkgconfig/bigmath.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/FindMPIR.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/FindGMP.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/FindMPDECIMAL.cmake
        DESTINATION
        ${INSTALL_CMAKE_DIR}
        )

install(
	TARGETS ${INSTALL_TARGETS}
	EXPORT ${PROJECT_NAME}-targets
	RUNTIME DESTINATION ${INSTALL_BIN_DIR}
	LIBRARY DESTINATION ${INSTALL_LIB_DIR}
	ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
	PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}
)

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/bigmath
	DESTINATION ${INSTALL_INCLUDE_DIR}
)

install(
	FILES
	${CMAKE_BINARY_DIR}/pkgconfig/bigmath.pc
	DESTINATION lib/pkgconfig
)

install(EXPORT ${PROJECT_NAME}-targets
        NAMESPACE ${PROJECT_NAME}::
        FILE "${PROJECT_NAME}-targets.cmake"
        DESTINATION ${INSTALL_CMAKE_DIR}
        )

set(PACKAGE_RELEASE 2)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Eduard Maximovich")
set(CPACK_PACKAGE_CONTACT "edward.vstock@gmail.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/edwardstock/bigmath")
set(CPACK_PACKAGE_VCS_URL "https://github.com/edwardstock/bigmath.git")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_DEBIAN_PACKAGE_SECTION "math")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libmpdec-dev (>=2.4.2), libgmp-dev (>=6.0.0)")
if (OS_NAME STREQUAL fedora)
	set(CPACK_RPM_PACKAGE_REQUIRES "mpdecimal-devel >= 2.4.2, mpir-devel >= 3.0.0")
else ()
	set(CPACK_RPM_PACKAGE_REQUIRES "mpdecimal-devel >= 2.4.2, gmp-devel >= 6.0.0")
endif ()


if ((IS_REDHAT OR IS_DEBIAN) AND NOT PACKAGE_ARCHIVE)
	if (IS_REDHAT)
		message(STATUS "Build package for redhat ${RH_MAJOR_VERSION}")

		get_target_property(target_type ${PROJECT_NAME} TYPE)
		if (target_type STREQUAL "EXECUTABLE")
			set(PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
		else ()
			set(PACKAGE_NAME "${CPACK_PACKAGE_NAME}-devel")
		endif ()

		set(PACKAGE_EXT ".rpm")
		set(CPACK_GENERATOR "RPM")
		set(CPACK_RPM_PACKAGE_NAME ${PACKAGE_NAME})
		set(CPACK_RPM_PACKAGE_ARCHITECTURE "${PROJECT_ARCH}")
		set(CPACK_RPM_PACKAGE_RELEASE "${PACKAGE_RELEASE}.${RH_MAJOR_NAME}")
		set(CPACK_RPM_PACKAGE_LICENSE "MIT")
		set(CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_HOMEPAGE_URL})
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

		# upload vars
		set(URL_SUFFIX "")
		set(REPO_NAME rh)
		set(TARGET_PATH "${OS_NAME}/${RH_MAJOR_VERSION}/${CMAKE_SYSTEM_PROCESSOR}/")
		set(UPLOAD_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}.rpm)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/package_upload.sh ${CMAKE_BINARY_DIR}/package_upload.sh @ONLY)
	else ()
		get_target_property(target_type ${PROJECT_NAME} TYPE)
		if (target_type STREQUAL "EXECUTABLE")
			set(PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
		else ()
			set(PACKAGE_NAME "lib${CPACK_PACKAGE_NAME}-dev")
		endif ()

		set(PACKAGE_EXT "")
		set(CPACK_GENERATOR "DEB")
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Eduard Maximovich <edward.vstock@gmail.com>")
		set(CPACK_DEBIAN_PACKAGE_RELEASE ${PACKAGE_RELEASE})
		set(CPACK_DEBIAN_PACKAGE_NAME "${PACKAGE_NAME}")
		set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${PROJECT_ARCH})
		set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${CPACK_PACKAGE_HOMEPAGE_URL}")
		set(CPACK_DEBIAN_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}_${PROJECT_VERSION}-${PACKAGE_RELEASE}_${OS_ARCH}.deb")

		# upload vars
		set(JFROG_OPTIONS "--deb \"${DEB_DIST_NAME}/main/${OS_ARCH}\"")
		set(REPO_NAME ${OS_NAME})
		set(TARGET_PATH "dists/${DEB_DIST_NAME}/main/")
		set(UPLOAD_FILE_NAME ${CPACK_DEBIAN_FILE_NAME})
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/package_upload.sh ${CMAKE_BINARY_DIR}/package_upload.sh @ONLY)
	endif ()

else ()
	set(PACKAGE_EXT ".sh")
	set(CPACK_GENERATOR "STGZ")
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
	set(UPLOAD_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}.sh)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/package_upload.sh ${CMAKE_BINARY_DIR}/package_upload.sh @ONLY)
endif ()
include(CPack)
