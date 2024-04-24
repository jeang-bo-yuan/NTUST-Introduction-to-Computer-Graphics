
#include <iostream>
#include <QApplication>
#include <QWidget>
#include "MainWindow.h"

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    MainWindow* w = new MainWindow;
    w->show();
    return app.exec();
}
