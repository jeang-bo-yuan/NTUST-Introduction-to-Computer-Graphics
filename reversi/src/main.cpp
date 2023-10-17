
#include <iostream>
#include <QApplication>
#include <QWidget>
#include "GameView.h"

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    GameView* v = new GameView;
    v->show();
    v->set_game(std::make_shared<Game>());
    return app.exec();
}
