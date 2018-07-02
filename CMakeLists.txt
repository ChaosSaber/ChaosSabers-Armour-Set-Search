cmake_minimum_required(VERSION 3.1)
Project(Dauntless_Armour_Set_Search VERSION 0.8.0 LANGUAGES CXX)
######## acquire QT
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
macro(qt5_install_dll DLL)
    # find the release *.dll file
    get_target_property(Qt5_${DLL}Location Qt5::${DLL} LOCATION)
	# message("Found ${DLL}: ${Qt5_${DLL}Location}")
	install(FILES ${Qt5_${DLL}Location} DESTINATION . CONFIGURATIONS Release RelWithDebInfo)
    # find the debug *d.dll file
    get_target_property(Qt5_${DLL}LocationDebug Qt5::${DLL} IMPORTED_LOCATION_DEBUG)
	install(FILES ${Qt5_${DLL}LocationDebug} DESTINATION . CONFIGURATIONS Debug)
endmacro()
qt5_install_dll(Core)
qt5_install_dll(Widgets)
qt5_install_dll(Gui)

add_subdirectory(code)
install(DIRECTORY language data DESTINATION .)
install(FILES README LICENSE LICENSE-3RD-PARTY DESTINATION .)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        SET(CMAKE_INSTALL_PREFIX "install" CACHE PATH "install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT) 
