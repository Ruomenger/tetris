#include "main_window.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("Tetris");
    setFixedSize(480, 680);
    setup_ui();
    start_game();
}

void MainWindow::setup_ui() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QHBoxLayout(central);

    game_widget_ = new GameWidget(game_, this);
    layout->addWidget(game_widget_);

    auto* side_panel = new QVBoxLayout();

    // 预览
    preview_widget_ = new PreviewWidget(game_, this);
    side_panel->addWidget(preview_widget_);

    // 信息
    score_label_ = new QLabel("Score: 0", this);
    level_label_ = new QLabel("Level: 1", this);
    lines_label_ = new QLabel("Lines: 0", this);
    side_panel->addWidget(score_label_);
    side_panel->addWidget(level_label_);
    side_panel->addWidget(lines_label_);

    // 按钮
    auto* pause_btn = new QPushButton("Pause", this);
    auto* restart_btn = new QPushButton("Restart", this);
    side_panel->addWidget(pause_btn);
    side_panel->addWidget(restart_btn);

    layout->addLayout(side_panel);

    // Timer
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this] {
        if (auto result = game_.tick(); !result) {
            game_over();
        }
        game_widget_->update();
        score_label_->setText(QString("Score: %1").arg(game_.score()));
        level_label_->setText(QString("Level: %1").arg(game_.level()));
        lines_label_->setText(QString("Lines: %1").arg(game_.lines_cleared()));
    });
}

void MainWindow::start_game() {
    std::ignore = game_.process(tetris::GameAction::Restart);
    timer_->start(game_.drop_interval());
}

void MainWindow::pause_game() {
    std::ignore = game_.process(tetris::GameAction::Pause);
    timer_->stop();
}

void MainWindow::game_over() {
    timer_->stop();
    score_label_->setText("Game Over!");
}
