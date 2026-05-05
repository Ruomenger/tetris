#include "game_widget.hpp"

#include <QKeyEvent>
#include <QPainter>

#include <tuple>

GameWidget::GameWidget(tetris::Game& game, QWidget* parent)
    : QWidget(parent), game_(game) {
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(kCellSize * tetris::kBoardWidth,
                  kCellSize * tetris::kBoardVisibleHeight);
}

QSize GameWidget::sizeHint() const {
    return {kCellSize * tetris::kBoardWidth,
            kCellSize * tetris::kBoardVisibleHeight};
}

void GameWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    painter.fillRect(rect(), QColor(30, 30, 30));

    // 网格线
    painter.setPen(QPen(QColor(60, 60, 60), 1));
    for (int x = 0; x <= tetris::kBoardWidth; ++x) {
        painter.drawLine(x * kCellSize, 0, x * kCellSize,
                         tetris::kBoardVisibleHeight * kCellSize);
    }
    for (int y = 0; y <= tetris::kBoardVisibleHeight; ++y) {
        painter.drawLine(0, y * kCellSize,
                         tetris::kBoardWidth * kCellSize,
                         y * kCellSize);
    }

    // 已锁定方块
    const auto& board = game_.board();
    for (int8_t row = 0; row < tetris::kBoardVisibleHeight; ++row) {
        for (int8_t col = 0; col < tetris::kBoardWidth; ++col) {
            auto cell = board.cell_at(
                {static_cast<int8_t>(row + tetris::kBoardBufferHeight), col});
            if (cell) {
                painter.fillRect(
                    col * kCellSize + 1, row * kCellSize + 1,
                    kCellSize - 2, kCellSize - 2,
                    QColor(100, 200, 100));
            }
        }
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    using tetris::GameAction;
    switch (event->key()) {
    case Qt::Key_Left:  std::ignore = game_.process(GameAction::MoveLeft); break;
    case Qt::Key_Right: std::ignore = game_.process(GameAction::MoveRight); break;
    case Qt::Key_Down:  std::ignore = game_.process(GameAction::SoftDrop); break;
    case Qt::Key_Space: std::ignore = game_.process(GameAction::HardDrop); break;
    case Qt::Key_Up:    std::ignore = game_.process(GameAction::RotateCW); break;
    case Qt::Key_Z:     std::ignore = game_.process(GameAction::RotateCCW); break;
    case Qt::Key_P:     std::ignore = game_.process(GameAction::Pause); break;
    case Qt::Key_R:     std::ignore = game_.process(GameAction::Restart); break;
    default: QWidget::keyPressEvent(event); return;
    }
    update();
}
