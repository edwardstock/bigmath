include(FeatureSummary)
include(FindLinuxPlatform)

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
install(
	DIRECTORY ${CMAKE_BINARY_DIR}/pkgconfig
	DESTINATION lib
)

if (ENABLE_SHARED AND EXISTS ${CMAKE_BINARY_DIR}/lib/lib${PROJECT_NAME}.a)
	install(
		FILES ${CMAKE_BINARY_DIR}/lib/lib${PROJECT_NAME}.a
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	)
endif ()

if (IS_REDHAT OR IS_DEBIAN)
	message(STATUS "RedHat or Debian packaging")
	set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
	set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
	set(CPACK_PACKAGE_VENDOR "Eduard Maximovich")
	set(CPACK_PACKAGE_CONTACT "edward.vstock@gmail.com")
	set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/edwardstock/bigmath")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ arbitrary precision float and integer numbers library")
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

	set(PACKAGE_RELEASE 0)
	if (IS_REDHAT)
		message(STATUS "Build package for redhat ${RH_MAJOR_VERSION}")

		## to build package with both version SO and A, you should first build static (default version without buildroot clean),
		## then build shared version with parameter -DENABLE_SHARED=On
		if (ENABLE_SHARED AND EXISTS ${CMAKE_BINARY_DIR}/lib/lib${PROJECT_NAME}.a)
			set(CPACK_RPM_USER_FILELIST "/usr/lib/lib${PROJECT_NAME}.a")
		endif ()

		set(CPACK_GENERATOR "RPM")
		set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-devel)
		set(CPACK_RPM_PACKAGE_ARCHITECTURE "${PROJECT_ARCH}")
		set(CPACK_RPM_PACKAGE_RELEASE "${PACKAGE_RELEASE}.${RH_MAJOR_NAME}")
		set(CPACK_RPM_PACKAGE_LICENSE "MIT")
		set(CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_HOMEPAGE_URL})
		set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")

		if (OS_NAME STREQUAL fedora)
			set(CPACK_RPM_PACKAGE_REQUIRES "mpdecimal >= 2.4.2, mpir >= 3.0.0")
		else ()
			set(CPACK_RPM_PACKAGE_REQUIRES "mpdecimal >= 2.4.2, gmp >= 6.0.0")
		endif ()


		set(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

		# upload vars
		# rh/bigmath/1.0.0/centos/7/x86_64/bigmath-devel-1.0.0-0.el7.x86_64.rpm
		set(URL_SUFFIX "")
		set(REPO_NAME rh)
		set(TARGET_PATH "${OS_NAME}/${RH_MAJOR_VERSION}/${CMAKE_SYSTEM_PROCESSOR}")
		set(UPLOAD_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}.rpm)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/package_upload.sh ${CMAKE_BINARY_DIR}/package_upload.sh @ONLY)
	else ()
		set(CPACK_GENERATOR "DEB")
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Eduard Maximovich <edward.vstock@gmail.com>")
		set(CPACK_DEBIAN_PACKAGE_RELEASE ${PACKAGE_RELEASE})
		set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}-dev")
		set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${PROJECT_ARCH})
		set(CPACK_DEBIAN_PACKAGE_DEPENDS "libmpdec-dev (>=2.4.2), libgmp-dev (>=6.0.0)")
		set(CPACK_DEBIAN_PACKAGE_SECTION "math")
		set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
		set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${CPACK_PACKAGE_HOMEPAGE_URL}")
		set(CPACK_DEBIAN_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}_${PROJECT_VERSION}-${PACKAGE_RELEASE}_${OS_ARCH}.deb")

		# -H "X-Bintray-Debian-Distribution: stretch"
		# -H "X-Bintray-Debian-Component: main"
		# -H "X-Bintray-Debian-Architecture: amd64"

		# upload vars
		set(URL_SUFFIX ";deb_distribution=${DEB_DIST_NAME};deb_component=main;deb_architecture=${OS_ARCH}")
		set(REPO_NAME ${OS_NAME})
		set(TARGET_PATH "dists/${DEB_DIST_NAME}/main")
		set(UPLOAD_FILE_NAME ${CPACK_DEBIAN_FILE_NAME})
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cfg/package_upload.sh ${CMAKE_BINARY_DIR}/package_upload.sh @ONLY)
	endif ()

endif ()
include(CPack)
