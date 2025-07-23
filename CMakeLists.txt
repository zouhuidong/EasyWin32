cmake_minimum_required(VERSION 3.10)
project(HiEasyX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 根 include 目录下的头文件
set(HIEX_ROOT_INCLUDE_HEADERS
    include/HiCanvas.h
    include/HiContainer.h
    include/HiDef.h
    include/HiDrawingProperty.h
    include/HiEasyX.h
    include/HiFPS.h
    include/HiGdiplus.h
    include/HiGif.h
    include/HiGraphicsUtils.h
    include/HiIcon.h
    include/HiMacro.h
    include/HiMiscUtils.h
    include/HiMouseDrag.h
    include/HiMusicMCI.h
    include/HiString.h
    include/HiWindow.h
)

# HiSysGUI 子目录下的头文件
set(HIEX_SYSGUI_INCLUDE_HEADERS
    include/HiSysGUI/HiSysGUI.h
    include/HiSysGUI/SysButton.h
    include/HiSysGUI/SysCheckBox.h
    include/HiSysGUI/SysComboBox.h
    include/HiSysGUI/SysControlBase.h
    include/HiSysGUI/SysEdit.h
    include/HiSysGUI/SysGroup.h
    include/HiSysGUI/SysGroupBox.h
    include/HiSysGUI/SysRadioButton.h
    include/HiSysGUI/SysStatic.h
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

# 构建静态库
add_library(HiEasyXLib STATIC 
	${HIEX_ALL_INCLUDE_HEADERS}		# 把头文件也加进来
	${HIEX_ALL_SRC_FILES}
)

# 设置库包含目录，PUBLIC 表示引用该库的目标也会继承这些目录
target_include_directories(HiEasyXLib PUBLIC 
    include
#    $<INSTALL_INTERFACE:include>  # 支持安装后使用
)

# 示例代码
add_executable(example_hello_world examples/helloworld.cpp)
target_link_libraries(example_hello_world HiEasyXLib)

