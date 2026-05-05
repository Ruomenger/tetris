#include "preview_widget.hpp"

#include <QPainter>

#include "colors.hpp"

PreviewWidget::PreviewWidget(tetris::Game& game, QWidget* parent) : QWidget(parent), game_(game)
{
    setFixedSize(kBoardPixel, kBoardPixel + 20);
}

QSize PreviewWidget::sizeHint() const
{
    return { kBoardPixel, kBoardPixel + 20 };
}

void PreviewWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(40, 40, 40));

    painter.setPen(QPen(QColor(100, 100, 100), 1));
    painter.setFont(QFont("monospace", 10));
    painter.drawText(QRect(0, 0, width(), 18), Qt::AlignCenter, "NEXT");

    auto next_type = game_.next_piece();
    if (!next_type.has_value())
        return;

    auto cells = tetris::Tetromino::cells_for(*next_type, tetris::Rotation::R0, { 0, 0 });
    if (cells.empty())
        return;

    // 计算边界以便居中
    int8_t min_row = 127;
    int8_t max_row = -128;
    int8_t min_col = 127;
    int8_t max_col = -128;
    for (auto& p : cells) {
        min_row = std::min(min_row, p.row);
        max_row = std::max(max_row, p.row);
        min_col = std::min(min_col, p.col);
        max_col = std::max(max_col, p.col);
    }

    int cell_w = max_col - min_col + 1;
    int cell_h = max_row - min_row + 1;
    int total_w = cell_w * kCellSize;
    int total_h = cell_h * kCellSize;
    int offset_x = (kBoardPixel - total_w) / 2;
    int offset_y = 20 + (kBoardPixel - total_h) / 2;

    auto color = piece_color(*next_type);
    for (auto& p : cells) {
        int x = offset_x + (p.col - min_col) * kCellSize;
        int y = offset_y + (p.row - min_row) * kCellSize;
        painter.fillRect(x + 1, y + 1, kCellSize - 2, kCellSize - 2, color);

        QColor lighter = color.lighter(130);
        painter.setPen(QPen(lighter, 1));
        painter.drawLine(x + 1, y + 1, x + kCellSize - 1, y + 1);
        painter.drawLine(x + 1, y + 1, x + 1, y + kCellSize - 1);
    }
}
