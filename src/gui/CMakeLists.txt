# ── tetris_app: GUI 可执行文件 ──

add_executable(tetris
    main.cpp
    main_window.cpp
    game_widget.cpp
    preview_widget.cpp
)

target_link_libraries(tetris PRIVATE
    libtetris
    Qt6::Widgets
)

target_compile_features(tetris PRIVATE cxx_std_23)

# ── Qt MOC ────────────────────────────────────────
set_target_properties(tetris PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# ── 警告 ──────────────────────────────────────────
target_compile_options(tetris PRIVATE
    -Wall -Wextra -Wpedantic
)

# ── Sanitizers ────────────────────────────────────
if(ENABLE_SANITIZERS)
    target_compile_options(tetris PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(tetris PRIVATE
        -fsanitize=address,undefined
    )
endif()
