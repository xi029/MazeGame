#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowTitle("A*迷宫小游戏 - 增强版");
    w.show();

    return app.exec();
}
