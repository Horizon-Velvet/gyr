cmake_minimum_required(VERSION 3.20)

project(look LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(look
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render.cpp
)

target_include_directories(look PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_compile_options(look PRIVATE
    -Oz
    -DNDEBUG
    -flto
    -ffunction-sections
    -fdata-sections
    -fno-rtti
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)

target_link_options(look PRIVATE
    -Oz
    -flto
    -fno-rtti
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wl,--gc-sections
    -Wl,--strip-all
    -Wl,--as-needed
)
