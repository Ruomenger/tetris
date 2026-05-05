# ── 测试可执行文件 ──

add_executable(tetris_tests
    test_types.cpp
    test_board.cpp
    test_tetromino.cpp
    test_game.cpp
    test_scoring.cpp
)

target_link_libraries(tetris_tests PRIVATE
    libtetris
    GTest::gtest
    GTest::gtest_main
)

target_compile_features(tetris_tests PUBLIC cxx_std_23)

target_include_directories(tetris_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
)

# ── 警告 ──────────────────────────────────────────
target_compile_options(tetris_tests PRIVATE
    -Wall -Wextra -Wpedantic
)

# ── Sanitizers ────────────────────────────────────
if(ENABLE_SANITIZERS)
    target_compile_options(tetris_tests PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(tetris_tests PRIVATE
        -fsanitize=address,undefined
    )
endif()

# ── Coverage ──────────────────────────────────────
if(ENABLE_COVERAGE)
    target_compile_options(tetris_tests PRIVATE --coverage)
    target_link_options(tetris_tests PRIVATE --coverage)
endif()

# ── 注册测试 ──────────────────────────────────────
include(GoogleTest)
gtest_discover_tests(tetris_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
