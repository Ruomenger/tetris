#include "main_window.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Tetris");
    setFixedSize(520, 680);
    setup_ui();
    start_game();
}

void MainWindow::setup_ui()
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QHBoxLayout(central);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    game_widget_ = new GameWidget(game_, this);
    layout->addWidget(game_widget_);

    auto* side_panel = new QVBoxLayout();
    side_panel->setSpacing(8);

    preview_widget_ = new PreviewWidget(game_, this);
    side_panel->addWidget(preview_widget_);

    auto title = new QLabel("TETRIS", this);
    title->setFont(QFont("monospace", 16, QFont::Bold));
    title->setAlignment(Qt::AlignCenter);
    side_panel->addWidget(title);

    side_panel->addSpacing(10);

    score_label_ = new QLabel("Score\n0", this);
    score_label_->setFont(QFont("monospace", 11));
    score_label_->setAlignment(Qt::AlignCenter);
    side_panel->addWidget(score_label_);

    level_label_ = new QLabel("Level\n1", this);
    level_label_->setFont(QFont("monospace", 11));
    level_label_->setAlignment(Qt::AlignCenter);
    side_panel->addWidget(level_label_);

    lines_label_ = new QLabel("Lines\n0", this);
    lines_label_->setFont(QFont("monospace", 11));
    lines_label_->setAlignment(Qt::AlignCenter);
    side_panel->addWidget(lines_label_);

    side_panel->addSpacing(10);

    auto* pause_btn = new QPushButton("Pause", this);
    auto* restart_btn = new QPushButton("Restart", this);
    pause_btn->setMinimumHeight(36);
    restart_btn->setMinimumHeight(36);
    side_panel->addWidget(pause_btn);
    side_panel->addWidget(restart_btn);

    side_panel->addSpacing(10);

    auto* controls = new QLabel(
        "← →  Move\n"
        "↑    Rotate\n"
        "↓    Soft Drop\n"
        "Space Hard Drop\n"
        "P    Pause\n"
        "R    Restart",
        this);
    controls->setFont(QFont("monospace", 9));
    controls->setStyleSheet("color: #888;");
    side_panel->addWidget(controls);

    side_panel->addStretch();
    layout->addLayout(side_panel);

    connect(pause_btn, &QPushButton::clicked, this, [this] {
        if (game_.state() == tetris::GameState::Playing) {
            std::ignore = game_.process(tetris::GameAction::Pause);
            timer_->stop();
        } else if (game_.state() == tetris::GameState::Paused) {
            std::ignore = game_.process(tetris::GameAction::Resume);
            timer_->start(game_.drop_interval());
        }
        game_widget_->update();
    });

    connect(restart_btn, &QPushButton::clicked, this, [this] { start_game(); });

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this] {
        if (game_.state() != tetris::GameState::Playing)
            return;

        auto result = game_.tick();
        if (!result.has_value()) {
            timer_->stop();
        }

        auto new_interval = game_.drop_interval();
        if (timer_->interval() != static_cast<int>(new_interval.count())) {
            timer_->setInterval(static_cast<int>(new_interval.count()));
        }

        score_label_->setText(QString("Score\n%1").arg(game_.score()));
        level_label_->setText(QString("Level\n%1").arg(game_.level()));
        lines_label_->setText(QString("Lines\n%1").arg(game_.lines_cleared()));

        game_widget_->update();
        preview_widget_->update();
    });
}

void MainWindow::start_game()
{
    std::ignore = game_.process(tetris::GameAction::Restart);
    timer_->start(game_.drop_interval());
    game_widget_->update();
    preview_widget_->update();
}
