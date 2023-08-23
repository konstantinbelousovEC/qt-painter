# @copyright Copyright (c) 2023 by Konstantin Belousov

cmake_minimum_required(VERSION 3.12)

project(qt_painter)

file(GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/*.h")
file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/*.cpp")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QRC_FILE_PATH ${CMAKE_SOURCE_DIR}/resources.qrc)
set(CMAKE_PREFIX_PATH "/Users/konstantinbelousov/usr_lib/protobuf/package;/Users/konstantinbelousov/Qt/6.5.0/macos")

#find_package(Protobuf REQUIRED)
#find_package(Threads REQUIRED)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
    REQUIRED)

#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS properties.proto rectangle.proto)

qt_add_resources(QRC_RESOURCES ${QRC_FILE_PATH})

#add_executable(qt_painter main.cpp ${HEADER_FILES} ${SOURCE_FILES} ${QRC_RESOURCES} ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(qt_painter main.cpp ${HEADER_FILES} ${SOURCE_FILES} ${QRC_RESOURCES})

#target_include_directories(qt_painter PUBLIC ${Protobuf_INCLUDE_DIR})
#target_include_directories(qt_painter PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(qt_painter
  Qt::Core
  Qt::Gui
  Qt::Widgets
  #"$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>"
  #Threads::Threads
)

#string(REPLACE "libprotobuf.a" "libprotobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
#string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
