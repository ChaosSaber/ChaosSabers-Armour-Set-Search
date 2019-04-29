
ADD_DEFINITIONS(-DPROGRAM_VERSION="${PROJECT_VERSION}")
SET(CMAKE_CXX_STANDARD 17)

SET(SRCS
	src/Dictionary.cpp
	src/gear/Armour.cpp
	src/gear/Armoury.cpp
	src/gear/Weapon.cpp
	src/gear/Cell.cpp
	src/gear/SkillInfo.cpp
	src/gear/Skill.cpp
	src/gear/SkillList.cpp
	src/gear/ArmourSet.cpp
	src/gear/CellList.cpp
	src/gear/SkillType.cpp
	src/main.cpp
	src/util/string.cpp
	src/ArmourSetSearch.cpp
	src/ui/mainwindow.cpp
	src/ui/ArmourSetView.cpp
	src/ui/SkillSelector.cpp
	src/ui/Translation.cpp
	src/Options.cpp
	src/ui/AdvancedSearch.cpp
	src/ui/About.cpp
	src/ui/License.cpp
	src/ui/CellWindow.cpp
	src/util/json.cpp
	src/util/files.cpp
	src/gear/Gear.cpp
	# src/FileDownloader.cpp
)

SET(HDRS
	include/Dictionary.hpp
	include/gear/Armour.hpp
	include/gear/Armoury.hpp
	include/gear/Weapon.hpp
	include/gear/SkillType.hpp
	include/gear/Elements.hpp
	include/gear/Cell.hpp
	include/gear/SkillInfo.hpp
	include/gear/Skill.hpp
	include/gear/Gear.hpp
	include/gear/SkillList.hpp
	include/gear/ArmourSet.hpp
	include/gear/CellList.hpp
	include/util/string.hpp
	include/ArmourSetSearch.hpp
	include/ui/mainwindow.hpp
	include/ui/ArmourSetView.hpp
	include/ui/SkillSelector.hpp
	include/ui/Translation.hpp
	include/Options.hpp
	include/ui/AdvancedSearch.hpp
	include/ui/About.hpp
	include/ui/License.hpp
	include/ui/CellWindow.hpp
	include/util/json.hpp
	include/util/files.hpp
	# include/FileDownloader.hpp
)

SET(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui/)

SET(UI
	ui/mainwindow.ui
	ui/ArmourSetView.ui
	ui/AdvancedSearch.ui
	ui/About.ui
	ui/License.ui
	ui/CellWindow.ui
)


include_directories(include/)

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS} ${UI})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Network Qt5::Concurrent)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
# set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)

IF(WIN32)
	# find the windeployqt binary
	find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
	if(NOT TOOL_WINDEPLOYQT)
		message(FATAL_ERROR "Could not find windeployqt")
	endif()
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
ENDIF(WIN32)
