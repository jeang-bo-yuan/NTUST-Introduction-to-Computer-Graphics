
#include <iostream>
#include <QApplication>
#include <QWidget>

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QWidget* w = new QWidget;
    w->show();
    return app.exec();
}
