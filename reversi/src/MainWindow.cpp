#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_game_ptr(std::make_shared<Game>())
{
    ui->setupUi(this);
    ui->view->set_game(m_game_ptr);
    this->update_info();

    connect(ui->view, &GameView::clicked_success, this, &MainWindow::update_info);
    connect(ui->menuRestart, &QMenu::aboutToShow, this, &MainWindow::restart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_info()
{
    if (m_game_ptr->is_dark_turn()) {
        ui->labelTurn->setText("Dark's\nTurn");
    }
    else {
        ui->labelTurn->setText("Light's\nTurn");
    }

    ui->lcdDark->display(m_game_ptr->get_dark_num());
    ui->lcdLight->display(m_game_ptr->get_light_num());

    switch (m_game_ptr->get_state()) {
    case Game::State::Playing:
        ui->statusbar->showMessage("Playing");
        break;
    case Game::State::Dark_Win:
        ui->statusbar->showMessage("Dark Wins");
        break;
    case Game::State::Light_Win:
        ui->statusbar->showMessage("Light Wins");
        break;
    case Game::State::Draw:
        ui->statusbar->showMessage("Draw");
        break;
    }
}

void MainWindow::restart()
{
    m_game_ptr->reset();
    ui->view->repaint();
    this->update_info();
}
