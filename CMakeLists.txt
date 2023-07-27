cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ANDROID_STL gnustl_static)
set(ANDROID_ABI armeabi)

project(starry_sky)

add_library(starry_sky SHARED src/starry_sky.cpp)

set(GD_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/gd-headers)
target_include_directories(starry_sky PRIVATE
    ${GD_HEADERS_DIR}/cocos/cocos2dx/include
    ${GD_HEADERS_DIR}/cocos/cocos2dx
    ${GD_HEADERS_DIR}/cocos/cocos2dx/platform/android
    ${GD_HEADERS_DIR}/cocos/cocos2dx/kazmath/include
    
    ${GD_HEADERS_DIR}/gd/sprite_nodes)

target_link_libraries(starry_sky ${CMAKE_CURRENT_SOURCE_DIR}/libgame.so)
