#pragma once

#include <QWidget>

#include "game.hpp"

class GameWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(tetris::Game& game, QWidget* parent = nullptr);

    static constexpr int kCellSize = 30;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    tetris::Game& game_;
};
