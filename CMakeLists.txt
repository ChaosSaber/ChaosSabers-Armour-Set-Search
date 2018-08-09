cmake_minimum_required(VERSION 3.1)
Project(ChaosSabers-Armour-Set-Search VERSION 0.10.1 LANGUAGES CXX)
######## acquire QT
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(OpenSSL REQUIRED)

SET(OPENSSL_ROOT_DIR ${OPENSSL_INCLUDE_DIR}\\..)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
find_library(SSL_EAY_DLL
	NAMES
		ssleay32
	PATHS
		${OPENSSL_ROOT_DIR}/bin
)
find_library(LIB_EAY_DLL
	NAMES
		libeay32
	PATHS
		${OPENSSL_ROOT_DIR}/bin
)
INSTALL(FILES ${SSL_EAY_DLL} ${LIB_EAY_DLL} DESTINATION .)

add_subdirectory(code)
install(DIRECTORY language data DESTINATION .)
install(FILES README.md LICENSE LICENSE-3RD-PARTY Troubleshooting.txt Changelog.txt DESTINATION .)

# SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
# SET(CMAKE_INSTALL_UCRT_LIBRARIES ON)
# include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
include(CPack)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        SET(CMAKE_INSTALL_PREFIX "install" CACHE PATH "install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT) 
