#pragma once

#include <QWidget>

#include "game.hpp"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(tetris::Game& game, QWidget* parent = nullptr);

    static constexpr int kCellSize = 30;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void draw_grid(QPainter& painter);
    void draw_locked_cells(QPainter& painter);
    void draw_current_piece(QPainter& painter);
    void draw_ghost_piece(QPainter& painter);
    void draw_cell(QPainter& painter, int8_t row, int8_t col, const QColor& color);

    tetris::Game& game_;
};
