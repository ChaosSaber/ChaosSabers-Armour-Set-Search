
ADD_DEFINITIONS(-DPROGRAM_VERSION="${PROJECT_VERSION}")

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
)

SET(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui/)

SET(UI
	ui/mainwindow.ui
	ui/ArmourSetView.ui
	ui/AdvancedSearch.ui
	ui/About.ui
	ui/License.ui
)


include_directories(include/)

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS} ${UI})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)