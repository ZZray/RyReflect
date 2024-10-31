cmake_minimum_required(VERSION 3.10)
project(RyReflect)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加选项以启用或禁用Qt支持
set(USE_QT OFF)

if(USE_QT)
    message("Enable Qt support")
    set(CMAKE_PREFIX_PATH "D:/Lib/Qt6.8/6.8.0/msvc2022_64")
    # 查找Qt包
    find_package(Qt6 COMPONENTS Core REQUIRED)
    if (NOT Qt6_FOUND)
        find_package(Qt5 5.15 COMPONENTS Core REQUIRED)
    endif()

    # 添加Qt定义
    add_definitions(-DRY_USE_QT)
endif()

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp RyReflect.h)
add_executable(Generate_${PROJECT_NAME} "generate.cpp")

if(USE_QT)
    # 链接Qt库
    if (Qt6_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
    endif()
endif()
