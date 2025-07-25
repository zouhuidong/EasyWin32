cmake_minimum_required(VERSION 3.10)
project(HiEasyX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 获取当前平台架构（x86或x64）
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_ARCH "x64")
else()
    set(PLATFORM_ARCH "x86")
endif()

# 定义输出路径模板（架构 -> 配置 -> 字符集）
set(OUTPUT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(LIB_OUTPUT_DIR ${OUTPUT_BASE_DIR}/lib/${PLATFORM_ARCH})
set(BIN_OUTPUT_DIR ${OUTPUT_BASE_DIR}/bin/$<CONFIG>/${PLATFORM_ARCH}) # config 指的是 x86, x64 等


# 根 include 目录下的头文件
set(HIEX_ROOT_INCLUDE_HEADERS
    include/HiEasyX.h
    include/HiEasyX/HiCanvas.h
    include/HiEasyX/HiContainer.h
    include/HiEasyX/HiDef.h
    include/HiEasyX/HiDrawingProperty.h
    include/HiEasyX/HiFPS.h
    include/HiEasyX/HiGdiplus.h
    include/HiEasyX/HiGif.h
    include/HiEasyX/HiGraphicsUtils.h
    include/HiEasyX/HiIcon.h
    include/HiEasyX/HiMacro.h
    include/HiEasyX/HiMiscUtils.h
    include/HiEasyX/HiMouseDrag.h
    include/HiEasyX/HiMusicMCI.h
    include/HiEasyX/HiString.h
    include/HiEasyX/HiWindow.h
)

# HiSysGUI 子目录下的头文件
set(HIEX_SYSGUI_INCLUDE_HEADERS
    include/HiEasyX/HiSysGUI/HiSysGUI.h
    include/HiEasyX/HiSysGUI/SysButton.h
    include/HiEasyX/HiSysGUI/SysCheckBox.h
    include/HiEasyX/HiSysGUI/SysComboBox.h
    include/HiEasyX/HiSysGUI/SysControlBase.h
    include/HiEasyX/HiSysGUI/SysEdit.h
    include/HiEasyX/HiSysGUI/SysGroup.h
    include/HiEasyX/HiSysGUI/SysGroupBox.h
    include/HiEasyX/HiSysGUI/SysRadioButton.h
    include/HiEasyX/HiSysGUI/SysStatic.h
)

# 源文件 - src根目录
set(HIEX_ROOT_SRC_FILES
    src/HiCanvas.cpp
    src/HiContainer.cpp
    src/HiDrawingProperty.cpp
    src/HiFPS.cpp
    src/HiGdiplus.cpp
    src/HiGif.cpp
    src/HiGraphicsUtils.cpp
    src/HiIcon.cpp
    src/HiMiscUtils.cpp
    src/HiMouseDrag.cpp
    src/HiMusicMCI.cpp
    src/HiWindow.cpp
)

# 源文件 - HiSysGUI子目录
set(HIEX_SYSGUI_SRC_FILES
    src/HiSysGUI/SysButton.cpp
    src/HiSysGUI/SysCheckBox.cpp
    src/HiSysGUI/SysComboBox.cpp
    src/HiSysGUI/SysControlBase.cpp
    src/HiSysGUI/SysEdit.cpp
    src/HiSysGUI/SysGroup.cpp
    src/HiSysGUI/SysGroupBox.cpp
    src/HiSysGUI/SysRadioButton.cpp
    src/HiSysGUI/SysStatic.cpp
)

# 所有头文件
set(HIEX_ALL_INCLUDE_HEADERS
    ${HIEX_ROOT_INCLUDE_HEADERS}
    ${HIEX_SYSGUI_INCLUDE_HEADERS}
)

# 所有头文件
set(HIEX_ALL_SRC_FILES
    ${HIEX_ROOT_SRC_FILES}
    ${HIEX_SYSGUI_SRC_FILES}
)

#file(GLOB_RECURSE HiEasyX_SRC_DIR 
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
#)


# 构建静态库 - Unicode版本（w后缀）
add_library(HiEasyXw STATIC 
    ${HIEX_ALL_INCLUDE_HEADERS}
    ${HIEX_ALL_SRC_FILES}
)
target_compile_definitions(HiEasyXw PRIVATE HIEASYX_LIB_EXPORT UNICODE _UNICODE)
set_target_properties(HiEasyXw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${LIB_OUTPUT_DIR}"    # 显式指定 Debug/Release 输出目录，让 Debug/Release 的放一起
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${LIB_OUTPUT_DIR}"
	DEBUG_POSTFIX "d"
)
target_include_directories(HiEasyXw PUBLIC include)


# 构建静态库 - 多字节版本（a后缀）
add_library(HiEasyXa STATIC 
    ${HIEX_ALL_INCLUDE_HEADERS}
    ${HIEX_ALL_SRC_FILES}
)
target_compile_definitions(HiEasyXa PRIVATE HIEASYX_LIB_EXPORT _MBCS)
set_target_properties(HiEasyXa PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${LIB_OUTPUT_DIR}"    # 显式指定 Debug/Release 输出目录，让 Debug/Release 的放一起
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${LIB_OUTPUT_DIR}"
	DEBUG_POSTFIX "d"
)
target_include_directories(HiEasyXa PUBLIC include)


# 定义添加示例的宏
macro(add_example example_name source_file)
    # Unicode版本
    add_executable(${example_name}_w ${source_file})
    target_link_libraries(${example_name}_w HiEasyXw)
    target_compile_definitions(${example_name}_w PRIVATE UNICODE _UNICODE)
    set_target_properties(${example_name}_w PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
        DEBUG_POSTFIX "d"
    )
    
    # 多字节版本
    add_executable(${example_name}_a ${source_file})
    target_link_libraries(${example_name}_a HiEasyXa)
    target_compile_definitions(${example_name}_a PRIVATE _MBCS)
    set_target_properties(${example_name}_a PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
        DEBUG_POSTFIX "d"
    )
endmacro()

# 使用宏添加示例代码
add_example(avoid-while-yangw80 examples/avoid-while-yangw80.cpp)
add_example(avoid-white-blocks examples/avoid-white-blocks.cpp)
add_example(chess-marathon examples/chess-marathon.cpp)

