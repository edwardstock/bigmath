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

	if (IS_REDHAT)
		set(RH_PACKAGE_RELEASE 0)
		message(STATUS "Build package for redhat el${RH_MAJOR_VERSION}")

		## to build package with both version SO and A, you should first build static (default version without buildroot clean),
		## then build shared version with parameter -DENABLE_SHARED=On
		if (ENABLE_SHARED AND EXISTS ${CMAKE_BINARY_DIR}/lib/lib${PROJECT_NAME}.a)

			set(CPACK_RPM_USER_FILELIST "/usr/lib/lib${PROJECT_NAME}.a")
		endif ()

		set(CPACK_GENERATOR "RPM")
		#		set(CPACK_RPM_CHANGELOG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/RELEASE_NOTES.md)
		set(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME}-devel)
		set(CPACK_RPM_PACKAGE_ARCHITECTURE "${PROJECT_ARCH}")
		set(CPACK_RPM_PACKAGE_RELEASE "${RH_PACKAGE_RELEASE}.el${RH_MAJOR_VERSION}")
		set(CPACK_RPM_PACKAGE_LICENSE "MIT")
		set(CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_HOMEPAGE_URL})
		set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
		set(CPACK_RPM_PACKAGE_REQUIRES "mpdecimal >= 2.4.2, gmp >= 6.0.0")

		set(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
	else ()

	endif ()

endif ()
include(CPack)
