#include <QApplication>

#include "main_window.hpp"

auto main(int argc, char* argv[]) -> int {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Tetris");
    QApplication::setApplicationVersion("0.1.0");

    MainWindow window;
    window.show();

    return QApplication::exec();
}
