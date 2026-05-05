#include "preview_widget.hpp"

#include <QPainter>

PreviewWidget::PreviewWidget(tetris::Game& game, QWidget* parent)
    : QWidget(parent), game_(game) {
    setFixedSize(kCellSize * 4 + 20, kCellSize * 4 + 20);
}

QSize PreviewWidget::sizeHint() const {
    return {kCellSize * 4 + 20, kCellSize * 4 + 20};
}

void PreviewWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(40, 40, 40));

    auto next = game_.next_piece();
    if (!next) return;

    painter.fillRect(10, 10, kCellSize * 4, kCellSize * 4,
                     QColor(30, 30, 30));
    // TODO: 绘制方块形状
}
