#pragma once

#include <QColor>

#include <array>

#include "types.hpp"

inline QColor piece_color(tetris::TetrominoType type)
{
    switch (type) {
        case tetris::TetrominoType::I:
            return { 0, 240, 240 };  // 青色
        case tetris::TetrominoType::O:
            return { 240, 240, 0 };  // 黄色
        case tetris::TetrominoType::T:
            return { 160, 0, 240 };  // 紫色
        case tetris::TetrominoType::S:
            return { 0, 240, 0 };  // 绿色
        case tetris::TetrominoType::Z:
            return { 240, 0, 0 };  // 红色
        case tetris::TetrominoType::J:
            return { 0, 0, 240 };  // 蓝色
        case tetris::TetrominoType::L:
            return { 240, 160, 0 };  // 橙色
    }
    return { 128, 128, 128 };
}

inline QColor ghost_color(tetris::TetrominoType type)
{
    auto c = piece_color(type);
    return QColor(c.red(), c.green(), c.blue(), 60);
}
