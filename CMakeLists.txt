cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ANDROID_STL gnustl_static)
set(ANDROID_ABI armeabi)
project(starry_sky)

add_library(starry_sky SHARED src/starry_sky.cpp src/settings.cpp src/ui/menu_settings.cpp src/ui/components/option.cpp
    src/modules/editor.cpp
    src/modules/misc.cpp
    include/dobby_wrapper/utils.cpp
    submodules/sqlite/sqlite3.c
    submodules/sqlite/shell.c)

set(GD_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/gd-headers)
target_include_directories(starry_sky PRIVATE
    ${GD_HEADERS_DIR}/cocos2d/cocos2dx/include
    ${GD_HEADERS_DIR}/cocos2d/cocos2dx
    ${GD_HEADERS_DIR}/cocos2d/cocos2dx/platform/android
    ${GD_HEADERS_DIR}/cocos2d/cocos2dx/kazmath/include
    ${GD_HEADERS_DIR}/robtop
    ${GD_HEADERS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/submodules/sqlite)

set(DOBBY_DEBUG OFF CACHE INTERNAL "" FORCE)
set(DOBBY_GENERATE_SHARED OFF CACHE INTERNAL "" FORCE)
add_subdirectory(submodules/dobby) # 16b99a55dc219114832c33f3258f48ca2958ed70
target_link_libraries(starry_sky dobby)

target_link_libraries(starry_sky ${CMAKE_CURRENT_SOURCE_DIR}/libgame.so log android)
