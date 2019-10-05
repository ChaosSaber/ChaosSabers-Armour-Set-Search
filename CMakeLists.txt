cmake_minimum_required(VERSION 3.8)
Project(ChaosSabers-Armour-Set-Search VERSION 0.12.5 LANGUAGES CXX)

###############################
# project options
###############################
option(${PROJECT_NAME}_BUILD_TESTS "Build the unittests for the library" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "install" CACHE PATH "install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT) 


###############################
# find_packages
###############################
######## acquire QT
find_package(Qt5Widgets 5.12 REQUIRED)
find_package(Qt5Network 5.12 REQUIRED)
find_package(Qt5Concurrent 5.12 REQUIRED)
#TODO: remove required qt version check and then search for correct open ssl version (from qt 5.12 open ssl 1.1 is required)
find_package(OpenSSL 1.1 REQUIRED)
set(OPENSSL_ROOT_DIR ${OPENSSL_INCLUDE_DIR}\\..)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
find_library(SSL_EAY_DLL
	NAMES
		libssl-1_1-x64
	PATHS
		${OPENSSL_ROOT_DIR}/bin
)
find_library(LIB_EAY_DLL
	NAMES
		libcrypto-1_1-x64
	PATHS
		${OPENSSL_ROOT_DIR}/bin
)
if(WIN32)
	# find the windeployqt binary
	find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
	if(NOT TOOL_WINDEPLOYQT)
		message(FATAL_ERROR "Could not find windeployqt")
	endif()
endif(WIN32)


###############################
# project library
###############################
set(PROJECT_LIB lib${PROJECT_NAME})
add_library(${PROJECT_LIB} STATIC)
target_sources(${PROJECT_LIB}
	PUBLIC
		${PROJECT_SOURCE_DIR}/code/lib/include/ArmourSetSearch.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/Dictionary.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/Options.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Armour.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/ArmourSet.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Armoury.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Cell.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/CellList.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Elements.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Gear.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Skill.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/SkillInfo.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/SkillList.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/SkillType.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/gear/Weapon.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/util/GreyCodeGenerator.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/util/files.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/util/json.hpp
		${PROJECT_SOURCE_DIR}/code/lib/include/util/string.hpp
		${PROJECT_SOURCE_DIR}/deps/hashidsxx/hashids.h
	PRIVATE
		${PROJECT_SOURCE_DIR}/code/lib/src/ArmourSetSearch.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/Dictionary.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/Options.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/Armour.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/ArmourSet.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/Armoury.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/Cell.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/CellList.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/Gear.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/Skill.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/SkillInfo.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/SkillList.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/SkillType.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/gear/Weapon.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/util/GreyCodeGenerator.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/util/files.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/util/json.cpp
		${PROJECT_SOURCE_DIR}/code/lib/src/util/string.cpp
		${PROJECT_SOURCE_DIR}/deps/hashidsxx/hashids.cpp
)
set_target_properties(${PROJECT_LIB} 
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
)
target_include_directories(${PROJECT_LIB} 
	PUBLIC 
		${PROJECT_SOURCE_DIR}/code/lib/include/ 
	PRIVATE
		${PROJECT_SOURCE_DIR}/deps/hashidsxx
)
target_link_libraries(${PROJECT_LIB} PUBLIC Qt5::Core)


###############################
# project programm
###############################
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PUBLIC
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/About.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/AdvancedSearch.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/ArmourSetView.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/CellDisplay.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/CellWindow.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/DetailedArmourSetView.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/ElementalLabel.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/GearView.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/License.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/LoadoutEntry.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/Loadouts.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/SkillLabel.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/SkillSelector.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/ui/mainwindow.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/util/Export.hpp
		# ${PROJECT_SOURCE_DIR}/code/ui/include/util/FileDownloader.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/util/Spacer.hpp
		${PROJECT_SOURCE_DIR}/code/ui/include/util/Translation.hpp
	PRIVATE
		${PROJECT_SOURCE_DIR}/code/ui/assets/Resource.qrc
		${PROJECT_SOURCE_DIR}/code/ui/src/main.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/About.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/AdvancedSearch.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/ArmourSetView.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/CellDisplay.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/CellWindow.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/DetailedArmourSetView.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/ElementalLabel.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/GearView.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/License.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/LoadoutEntry.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/Loadouts.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/SkillLabel.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/SkillSelector.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/ui/mainwindow.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/util/Export.cpp
		# ${PROJECT_SOURCE_DIR}/code/ui/src/util/FileDownloader.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/util/Spacer.cpp
		${PROJECT_SOURCE_DIR}/code/ui/src/util/Translation.cpp
		${PROJECT_SOURCE_DIR}/code/ui/ui/About.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/AdvancedSearch.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/ArmourSetView.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/CellDisplay.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/CellWindow.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/DetailedArmourSetView.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/ElementalLabel.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/GearView.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/License.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/Loadouts.ui
		${PROJECT_SOURCE_DIR}/code/ui/ui/mainwindow.ui
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROGRAM_VERSION="${PROJECT_VERSION}")
set_target_properties(${PROJECT_NAME} 
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		AUTOMOC ON  # Run moc automatically when needed
		AUTOUIC ON  # Create code from a list of Qt designer ui files
		AUTORCC ON
		AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/code/ui/ui/
		# WIN32_EXECUTABLE true
)
target_include_directories(${PROJECT_LIB} PUBLIC ${PROJECT_SOURCE_DIR}/code/ui/include/)
target_link_libraries(${PROJECT_NAME} 
	PRIVATE 
		${PROJECT_LIB}
		Qt5::Widgets 
		Qt5::Network 
		Qt5::Concurrent
)


###############################
# project unittest
###############################
if(${${PROJECT_NAME}_BUILD_TESTS})
	#[[ TODO
		add a testing framework like gtest or isolator++ (https://www.typemock.com/isolatorpp-product-page)
		add some proper unittests (GreyCodeGenerator already contains some tests)
	]]	
endif()


###############################
# installation
###############################
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(FILES ${SSL_EAY_DLL} ${LIB_EAY_DLL} DESTINATION .)
install(DIRECTORY language data DESTINATION .)
install(FILES README.md LICENSE LICENSE-3RD-PARTY Troubleshooting.txt Changelog.txt DESTINATION .)
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND rmdir /S /Q "${CMAKE_CURRENT_BINARY_DIR}/Qt_$<CONFIG>/"
		COMMAND "${TOOL_WINDEPLOYQT}"
				--dir "${CMAKE_CURRENT_BINARY_DIR}/Qt_$<CONFIG>/"
				--verbose 0
				--no-compiler-runtime
				--no-angle
				--no-opengl-sw
				\"$<TARGET_FILE:${PROJECT_NAME}>\"
				
    )
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Qt_$<CONFIG>/ DESTINATION .)
endif()

# SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
# SET(CMAKE_INSTALL_UCRT_LIBRARIES ON)
# include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
include(CPack)
