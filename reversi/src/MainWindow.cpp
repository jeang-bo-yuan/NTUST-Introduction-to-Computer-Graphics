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
    connect(ui->buttonToggleHint, &QPushButton::toggled, ui->view, &GameView::toggle_hint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_info()
{
    ui->lcdDark->display(m_game_ptr->get_dark_num());
    ui->lcdLight->display(m_game_ptr->get_light_num());

    switch (m_game_ptr->get_state()) {
    case Game::State::Playing:
        ui->statusbar->showMessage("Playing");
        if (m_game_ptr->is_dark_turn()) {
            ui->stackState->setCurrentWidget(ui->pageDarkTurn);
        }
        else {
            ui->stackState->setCurrentWidget(ui->pageLightTurn);
        }
        break;
    case Game::State::Dark_Win:
        ui->statusbar->showMessage("Dark Wins");
        ui->stackState->setCurrentWidget(ui->pageDarkWin);
        break;
    case Game::State::Light_Win:
        ui->statusbar->showMessage("Light Wins");
        ui->stackState->setCurrentWidget(ui->pageLightWin);
        break;
    case Game::State::Draw:
        ui->statusbar->showMessage("Draw");
        ui->stackState->setCurrentWidget(ui->pageDraw);
        break;
    }
}

void MainWindow::restart()
{
    m_game_ptr->reset();
    ui->view->repaint();
    this->update_info();
}
