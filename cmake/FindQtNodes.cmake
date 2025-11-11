# FindQtNodes.cmake - 简洁版本
if(DEFINED ENV{QTNODES_ROOT})
    set(QTNODES_ROOT $ENV{QTNODES_ROOT})
else()
    set(QTNODES_ROOT "E:/Environment/QtNode")
endif()

# 根据构建类型设置库路径
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIB_DIR "lib/Debug")
    set(LIB_NAMES QtNodesd QtNodes)
else()
    set(LIB_DIR "lib/Release")
    set(LIB_NAMES QtNodes)
endif()

# 查找头文件
find_path(QTNODES_INCLUDE_DIR
        NAMES QtNodes/DataFlowGraphModel
        HINTS ${QTNODES_ROOT}
        PATH_SUFFIXES include
)

# 直接查找对应构建类型的库文件
find_library(QTNODES_LIBRARY
        NAMES ${LIB_NAMES}
        HINTS ${QTNODES_ROOT}
        PATH_SUFFIXES ${LIB_DIR} lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtNodes
        REQUIRED_VARS QTNODES_LIBRARY QTNODES_INCLUDE_DIR
)

if(QtNodes_FOUND AND NOT TARGET QtNodes::QtNodes)
    add_library(QtNodes::QtNodes UNKNOWN IMPORTED)
    set_target_properties(QtNodes::QtNodes PROPERTIES
            IMPORTED_LOCATION "${QTNODES_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${QTNODES_INCLUDE_DIR}"
    )
endif()