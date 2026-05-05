# ── libtetris: 核心逻辑库 (纯 C++, 无 Qt 依赖) ──

add_library(libtetris STATIC)

target_sources(libtetris PRIVATE
    tetromino.cpp
    board.cpp
    game.cpp
    scoring.cpp
)

target_include_directories(libtetris PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(libtetris PUBLIC cxx_std_23)

# ── Sanitizers ──────────────────────────────────
if(ENABLE_SANITIZERS)
    target_compile_options(libtetris PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(libtetris PRIVATE
        -fsanitize=address,undefined
    )
endif()

# ── Coverage ────────────────────────────────────
if(ENABLE_COVERAGE)
    target_compile_options(libtetris PRIVATE --coverage)
    target_link_options(libtetris PRIVATE --coverage)
endif()

# ── 警告 ────────────────────────────────────────
target_compile_options(libtetris PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
)
