cmake_minimum_required(VERSION 3.5)

project(QJForm LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.
option(QJFORM_BUILD_EXE "Build the QJForm executable" TRUE)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_library(QJForm_lib
    src/qjform.cpp
    include/QJForm/qjform.h
    src/ToggleSwitch.cpp
    include/QJForm/ToggleSwitch.h
    src/CollapsableWidget.cpp
    include/QJForm/CollapsableWidget.h
)
target_include_directories( QJForm_lib PUBLIC include )

add_library( QJForm::QJForm ALIAS QJForm_lib  )
target_link_libraries(QJForm_lib PUBLIC Qt5::Widgets)


if( QJFORM_BUILD_EXE )
    add_executable(QJForm main.cpp )
    target_link_libraries(QJForm PRIVATE Qt5::Widgets QJForm::QJForm )
endif()
