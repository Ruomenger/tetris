#include "game_widget.hpp"

#include <QKeyEvent>
#include <QPainter>

#include <tuple>

#include "colors.hpp"

GameWidget::GameWidget(tetris::Game& game, QWidget* parent) : QWidget(parent), game_(game)
{
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(kCellSize * tetris::kBoardWidth, kCellSize * tetris::kBoardVisibleHeight);
}

QSize GameWidget::sizeHint() const
{
    return { kCellSize * tetris::kBoardWidth, kCellSize * tetris::kBoardVisibleHeight };
}

void GameWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(30, 30, 30));

    draw_grid(painter);
    draw_locked_cells(painter);

    if (game_.current_piece().has_value()) {
        draw_ghost_piece(painter);
        draw_current_piece(painter);
    }

    if (game_.state() == tetris::GameState::Paused) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("monospace", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "PAUSED");
    } else if (game_.state() == tetris::GameState::GameOver) {
        painter.fillRect(rect(), QColor(0, 0, 0, 160));
        painter.setPen(Qt::red);
        painter.setFont(QFont("monospace", 20, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER");
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event)
{
    using tetris::GameAction;

    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
        case Qt::Key_Left:
            std::ignore = game_.process(GameAction::MoveLeft);
            break;
        case Qt::Key_Right:
            std::ignore = game_.process(GameAction::MoveRight);
            break;
        case Qt::Key_Down:
            std::ignore = game_.process(GameAction::SoftDrop);
            break;
        case Qt::Key_Space:
            std::ignore = game_.process(GameAction::HardDrop);
            break;
        case Qt::Key_Up:
            std::ignore = game_.process(GameAction::RotateCW);
            break;
        case Qt::Key_Z:
            std::ignore = game_.process(GameAction::RotateCCW);
            break;
        case Qt::Key_P:
            std::ignore = game_.process(GameAction::Pause);
            break;
        case Qt::Key_R:
            std::ignore = game_.process(GameAction::Restart);
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
    update();
}

void GameWidget::draw_grid(QPainter& painter)
{
    painter.setPen(QPen(QColor(50, 50, 50), 1));
    for (int x = 0; x <= tetris::kBoardWidth; ++x) {
        painter.drawLine(x * kCellSize, 0, x * kCellSize, height());
    }
    for (int y = 0; y <= tetris::kBoardVisibleHeight; ++y) {
        painter.drawLine(0, y * kCellSize, width(), y * kCellSize);
    }
}

void GameWidget::draw_locked_cells(QPainter& painter)
{
    const auto& board = game_.board();
    for (int8_t row = 0; row < tetris::kBoardVisibleHeight; ++row) {
        for (int8_t col = 0; col < tetris::kBoardWidth; ++col) {
            auto cell =
                board.cell_at({ static_cast<int8_t>(row + tetris::kBoardBufferHeight), col });
            if (cell.has_value()) {
                draw_cell(painter, row, col, piece_color(*cell));
            }
        }
    }
}

void GameWidget::draw_current_piece(QPainter& painter)
{
    const auto& piece = game_.current_piece();
    if (!piece.has_value())
        return;

    auto color = piece_color(piece->type());
    for (auto& p : piece->cells()) {
        int8_t visible_row = static_cast<int8_t>(p.row - tetris::kBoardBufferHeight);
        if (visible_row >= 0 && visible_row < tetris::kBoardVisibleHeight) {
            draw_cell(painter, visible_row, p.col, color);
        }
    }
}

void GameWidget::draw_ghost_piece(QPainter& painter)
{
    const auto& piece = game_.current_piece();
    if (!piece.has_value())
        return;

    auto ghost = game_.ghost_position();
    if (ghost.empty())
        return;

    auto color = ghost_color(piece->type());
    for (auto& p : ghost) {
        int8_t visible_row = static_cast<int8_t>(p.row - tetris::kBoardBufferHeight);
        if (visible_row >= 0 && visible_row < tetris::kBoardVisibleHeight) {
            draw_cell(painter, visible_row, p.col, color);
        }
    }
}

void GameWidget::draw_cell(QPainter& painter, int8_t row, int8_t col, const QColor& color)
{
    int x = col * kCellSize;
    int y = row * kCellSize;
    int pad = 1;

    painter.fillRect(x + pad, y + pad, kCellSize - pad * 2, kCellSize - pad * 2, color);

    // 高光效果
    QColor lighter = color.lighter(130);
    painter.setPen(QPen(lighter, 2));
    painter.drawLine(x + pad, y + pad, x + kCellSize - pad, y + pad);
    painter.drawLine(x + pad, y + pad, x + pad, y + kCellSize - pad);

    QColor darker = color.darker(130);
    painter.setPen(QPen(darker, 2));
    painter.drawLine(x + pad, y + kCellSize - pad, x + kCellSize - pad, y + kCellSize - pad);
    painter.drawLine(x + kCellSize - pad, y + pad, x + kCellSize - pad, y + kCellSize - pad);
}
