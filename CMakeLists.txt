cmake_minimum_required(VERSION 3.14)

project(qtontotaskbar LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(qtontotaskbar SHARED
  src/qtontotaskbar_global.h
  src/qtontotaskbar.cpp
  src/qtontotaskbar.h
)

target_link_libraries(qtontotaskbar PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_compile_definitions(qtontotaskbar PRIVATE QTONTOTASKBAR_LIBRARY)


if (WIN32)
    message("CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND python "${CMAKE_SOURCE_DIR}/script/output-cpplib.py" win32msvc) # PRE_BUILD | PRE_LINK| POST_BUILD
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND python "${CMAKE_SOURCE_DIR}/script/output-cpplib.py" win32mingw) # PRE_BUILD | PRE_LINK| POST_BUILD
    endif ()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
elseif (UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND python3 "${CMAKE_SOURCE_DIR}/script/output-cpplib.py" linux64)

endif()
