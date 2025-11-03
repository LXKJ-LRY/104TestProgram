cmake_minimum_required(VERSION 3.16)

project(testProgram VERSION 0.3 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Sql Network)

if(UNIX) #ubuntu20.04 needed
    find_package(Threads REQUIRED)
endif()

set(TS_FILES testProgram_zh_CN.ts)
set(TRANSLATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}  # 主目录
    ${CMAKE_CURRENT_SOURCE_DIR}/IEC104  # 子目录（如有）
)

set(Libiec60870Path ${CMAKE_CURRENT_SOURCE_DIR}/libiec60870)
include_directories(${Libiec60870Path}/include)
link_directories(${Libiec60870Path}/lib)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(testProgram
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET testProgram APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(testProgram SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        find_package(Qt5 REQUIRED COMPONENTS Widgets)
        find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Core)

        add_executable(testProgram
            ${PROJECT_SOURCES}
            ${QM_FILES}
            dbdatahandler.h dbdatahandler.cpp

            iec104controller.h iec104controller.cpp
            IEC104/iec104master.h IEC104/iec104master.cpp
            testlogmanager.h testlogmanager.cpp
            # IEC104/iec104masterstrategy.h
            # IEC104/remotecontrollockmasterstrategy.h IEC104/remotecontrollockmasterstrategy.cpp
            # IEC104/iec104masterstrategyfactory.h IEC104/iec104masterstrategyfactory.cpp





        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

message(STATUS "QM_FILES: ${QM_FILES}")

target_link_libraries(testProgram PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(testProgram PRIVATE Qt5::Widgets)
target_link_libraries(testProgram PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(testProgram PRIVATE Qt5::Sql)
target_link_libraries(testProgram PRIVATE Qt5::Core)
target_link_libraries(testProgram PRIVATE lib60870)

if(UNIX)
    target_link_libraries(testProgram PRIVATE Threads::Threads)
target_link_libraries(testProgram PRIVATE Qt5::Core)
endif()

add_custom_target(translations ALL
    DEPENDS ${QM_FILES}
    COMMENT "Generating translation files to ${QM_OUTPUT_DIR}"
)


add_dependencies(testProgram translations)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.testProgram)
endif()
set_target_properties(testProgram PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS testProgram
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(testProgram)
endif()

