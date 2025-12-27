cmake_minimum_required(VERSION 3.16)

project(BikiniBottom2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES BikiniBottom2_uk_UA.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BikiniBottom2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        AI.h
        Cell.h
        Unit.h
        Map.h
        UnitFabric.h
        AI.cpp
        Cell.cpp
        Map.cpp
        Unit.cpp
        UnitFabric.cpp
        SettingsWidget.h SettingsWidget.cpp
        PauseWidget.h PauseWidget.cpp
        Fight.h Fight.cpp
        GameSelectionWidget.h GameSelectionWidget.cpp
        RandomGenerator.h
        RandomGenerator.cpp
        HeroWidget.h HeroWidget.cpp
        GameConstants.h
        HexItem.h HexItem.cpp
        GameScene.h GameScene.cpp
        GameView.h GameView.cpp
        TextureManager.h TextureManager.cpp
        PerlinNoise.h
        SpellManager.h SpellManager.cpp
        LevelUpSystem.h
        LevelUpWidget.h
        SkillTreeSystem.h
        SkillTreeWidget.h
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

target_link_libraries(BikiniBottom2 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

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
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
target_link_libraries(BikiniBottom2 PRIVATE Qt6::Multimedia)
