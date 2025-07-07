cmake_minimum_required(VERSION 3.8)  # 不要修改
project(CodeCraft)                   # 不要修改

set(CMAKE_CXX_STANDARD      17)      # 不要修改
set(CMAKE_C_STANDARD        11)      # 不要修改

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/) # 不要修改
option(BUILD_SHARED_LIBS    ""  OFF) # 不要修改

include_directories(${PROJECT_SOURCE_DIR})

# 将拆分后的源文件列出来
set(SRC_FILES
    main.cpp
    globals.cpp
    timestamp.cpp
    delete_action.cpp
    write_action.cpp
    read_action.cpp
    utils.cpp
)

add_executable(code_craft ${SRC_FILES}) # 不要修改可执行文件名称

#if (NOT WIN32)
#    target_link_libraries(code_craft pthread rt m)
#endif()
