#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QTimer>

#include "game.hpp"
#include "game_widget.hpp"
#include "preview_widget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    void setup_ui();
    void start_game();

    tetris::Game game_;
    GameWidget* game_widget_{ nullptr };
    PreviewWidget* preview_widget_{ nullptr };
    QLabel* score_label_{ nullptr };
    QLabel* level_label_{ nullptr };
    QLabel* lines_label_{ nullptr };
    QTimer* timer_{ nullptr };
};
