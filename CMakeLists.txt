cmake_minimum_required(VERSION 3.8)
project(NodeEditorDemo)

set(CMAKE_CXX_STANDARD 17)

# 必须设置这些选项
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_definitions(-DNODE_EDITOR_SHARED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 定义变量
set(INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB SRC_FILES "*.cpp")
set(THIRD_PARTY_LIBS "")

# 查找依赖
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
find_package(QtNodes REQUIRED)

if(QtNodes_FOUND)
    list(APPEND INC_DIRS ${QTNODES_INCLUDE_DIRS})
    list(APPEND THIRD_PARTY_LIBS QtNodes::QtNodes)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 配置构建
include_directories(${INC_DIRS})

add_executable(nodeeditor_demo
        ${SRC_FILES})

target_link_libraries(nodeeditor_demo
        Qt5::Core Qt5::Widgets Qt5::Gui
        ${THIRD_PARTY_LIBS}
)

# 打印信息
message(STATUS "===========================================")
message(STATUS "开始配置 NodeEditorDemo 项目")
message(STATUS "项目名称: ${PROJECT_NAME}")
message(STATUS "源码目录: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "构建目录: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "生成器: ${CMAKE_GENERATOR}")

message(STATUS "包含目录:")
foreach(dir ${INC_DIRS})
    message(STATUS "  - ${dir}")
endforeach()

message(STATUS "源文件:")
foreach(src ${SRC_FILES})
    message(STATUS "  - ${src}")
endforeach()

message(STATUS "三方库:")
foreach(lib ${THIRD_PARTY_LIBS})
    message(STATUS "  - ${lib}")
endforeach()

message(STATUS "===========================================")