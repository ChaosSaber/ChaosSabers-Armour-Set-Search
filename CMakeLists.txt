cmake_minimum_required(VERSION 3.1)
Project(ChaosSabers-Armour-Set-Search VERSION 0.8.3 LANGUAGES CXX)
######## acquire QT
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

# SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH}")
# message(${CMAKE_MODULE_PATH})
# include(windeployqt)

add_subdirectory(code)
install(DIRECTORY language data DESTINATION .)
install(FILES README.md LICENSE LICENSE-3RD-PARTY DESTINATION .)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
include(CPack)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        SET(CMAKE_INSTALL_PREFIX "install" CACHE PATH "install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT) 
