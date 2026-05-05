#pragma once

#include <QWidget>

#include "game.hpp"

class PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWidget(tetris::Game& game, QWidget* parent = nullptr);

    static constexpr int kCellSize = 22;
    static constexpr int kBoardPixel = kCellSize * 4;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    tetris::Game& game_;
};
