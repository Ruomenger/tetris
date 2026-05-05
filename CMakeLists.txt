cmake_minimum_required(VERSION 3.25)

project(
    tetris
    VERSION 0.1.0
    DESCRIPTION "Classic Tetris game built with C++23 and Qt 6.10"
    LANGUAGES CXX
)

# ── C++ 标准 ──────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── 导出编译命令 (供 clangd/clang-tidy 使用) ─────
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── 选项 ──────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# ── 查找 Qt6 ──────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ── GTest ──────────────────────────────────────────
if(BUILD_TESTS)
    include(CTest)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW ON
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

# ── 子目录 ────────────────────────────────────────
add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
