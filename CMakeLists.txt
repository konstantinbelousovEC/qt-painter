# @copyright Copyright (c) 2023 by Konstantin Belousov

cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

project(qt_painter)

file(GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/source/*.cpp")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QRC_FILE_PATH ${CMAKE_SOURCE_DIR}/resources.qrc)
set(CMAKE_PREFIX_PATH "/Users/konstantinbelousov/usr_lib/protobuf/package;/Users/konstantinbelousov/Qt/6.5.0/macos")

find_package(Qt6 COMPONENTS
             Core
             Gui
             Widgets REQUIRED)

qt_add_resources(QRC_RESOURCES ${QRC_FILE_PATH})

add_executable(qt_painter main.cpp ${HEADER_FILES} ${SOURCE_FILES} ${QRC_RESOURCES})

target_link_libraries(qt_painter
  Qt::Core
  Qt::Gui
  Qt::Widgets
)

#configure_file(${CMAKE_SOURCE_DIR}/toolbarbtnstylesheet.qss ${CMAKE_BINARY_DIR}/toolbarbtnstylesheet.qss COPYONLY)
