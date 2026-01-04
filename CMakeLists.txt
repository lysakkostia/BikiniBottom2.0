cmake_minimum_required(VERSION 3.16)

project(BikiniBottom2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Multimedia MultimediaWidgets)

include_directories(src)

set(TS_FILES src/BikiniBottom2_uk_UA.ts)
set(RESOURCES resourсes/resources.qrc)

set(PROJECT_SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/AI.h
        src/AI.cpp
        src/Cell.h
        src/Cell.cpp
        src/Unit.h
        src/Unit.cpp
        src/Map.h
        src/Map.cpp
        src/UnitFabric.h
        src/UnitFabric.cpp
        src/GameConstants.h
        src/HexItem.h
        src/HexItem.cpp
        src/GameScene.h
        src/GameScene.cpp
        src/GameView.h
        src/GameView.cpp
        src/TextureManager.h
        src/TextureManager.cpp
        src/SpellManager.h
        src/SpellManager.cpp
        src/PerlinNoise.h
        src/RandomGenerator.h
        src/Fight.h
        src/Fight.cpp
        src/SettingsWidget.h
        src/SettingsWidget.cpp
        src/PauseWidget.h
        src/PauseWidget.cpp
        src/GameSelectionWidget.h
        src/GameSelectionWidget.cpp
        src/HeroWidget.h
        src/HeroWidget.cpp
        src/LevelUpSystem.h
        src/LevelUpWidget.h
        src/LevelUpWidget.cpp
        src/SkillTreeSystem.h
        src/SkillTreeWidget.h
        src/SkillTreeWidget.cpp
        src/CampfireWidget.h
        src/CampfireWidget.cpp
        src/NPCWidget.h
        src/NPCWidget.cpp
        src/TutorialWidget.h
        src/TutorialWidget.cpp

        ${TS_FILES}
        ${RESOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BikiniBottom2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BikiniBottom2 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(BikiniBottom2 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BikiniBottom2
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(BikiniBottom2 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BikiniBottom2)
endif()
set_target_properties(BikiniBottom2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BikiniBottom2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BikiniBottom2)
endif()
