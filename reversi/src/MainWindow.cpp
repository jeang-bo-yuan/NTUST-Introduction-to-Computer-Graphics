#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_game_ptr(std::make_shared<Game>())
{
    ui->setupUi(this);
    ui->view->set_game(m_game_ptr);
    this->update_info();

    connect(ui->view, &GameView::clicked_success, this, &MainWindow::update_info);
    ui->menuFrame->layout()->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    connect(ui->buttonRestart, &QPushButton::clicked, this, &MainWindow::restart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_info()
{
    ui->labelDarkTurn->hide();
    ui->labelLightTurn->hide();
    ui->labelDarkWin->hide();
    ui->labelLightWin->hide();
    ui->labelDraw->hide();

    ui->lcdDark->display(m_game_ptr->get_dark_num());
    ui->lcdLight->display(m_game_ptr->get_light_num());

    switch (m_game_ptr->get_state()) {
    case Game::State::Playing:
        ui->statusbar->showMessage("Playing");
        if (m_game_ptr->is_dark_turn()) {
            ui->labelDarkTurn->show();
        }
        else {
            ui->labelLightTurn->show();
        }
        break;
    case Game::State::Dark_Win:
        ui->statusbar->showMessage("Dark Wins");
        ui->labelDarkWin->show();
        break;
    case Game::State::Light_Win:
        ui->statusbar->showMessage("Light Wins");
        ui->labelLightWin->show();
        break;
    case Game::State::Draw:
        ui->statusbar->showMessage("Draw");
        ui->labelDraw->show();
        break;
    }
}

void MainWindow::restart()
{
    m_game_ptr->reset();
    ui->view->repaint();
    this->update_info();
}
