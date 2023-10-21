#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_game_ptr(std::make_shared<Game>())
{
    ui->setupUi(this);
    ui->view->set_game(m_game_ptr);
    this->update_info();

    // 遊戲介面
    connect(ui->view, &GameView::clicked_success, this, &MainWindow::update_info);

    // 上方的menu
    ui->menuFrame->layout()->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    connect(ui->buttonRestart, &QPushButton::clicked, this, &MainWindow::restart);
    connect(ui->buttonToggleHint, &QPushButton::toggled, ui->view, &GameView::toggle_hint);
    connect(ui->buttonPalette, &QPushButton::toggled, this, &MainWindow::toggle_palette);

    // 初始化Palette
    ui->BG_display->set_color_ub(ui->view->m_color.BG);
    ui->Hint_display->set_color_ub(ui->view->m_color.hint);
    ui->Dark_display->set_color_ub(ui->view->m_color.dark);
    ui->Light_display->set_color_ub(ui->view->m_color.light);
    ui->Border_display->set_color_ub(ui->view->m_color.border);

    // 連接Palette的按鈕
    connect(ui->buttonSelectBGColor, &QPushButton::clicked, this, &MainWindow::choose_BG_color);
    connect(ui->buttonSelectHintColor, &QPushButton::clicked, this, &MainWindow::choose_hint_color);
    connect(ui->buttonSelectDarkColor, &QPushButton::clicked, this, &MainWindow::choose_dark_color);
    connect(ui->buttonSelectLightColor, &QPushButton::clicked, this, &MainWindow::choose_light_color);
    connect(ui->buttonSelectBorderColor, &QPushButton::clicked, this, &MainWindow::choose_border_color);
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

void MainWindow::toggle_palette(bool open)
{
    if (open)
        ui->stackSub->setCurrentWidget(ui->pagePalette);
    else
        ui->stackSub->setCurrentWidget(ui->pageInfo);
}

void MainWindow::choose_BG_color()
{
    QColor choose = QColorDialog::getColor(ui->BG_display->get_color(), nullptr, "Background");
    if (choose.isValid()) {
        ui->BG_display->set_color(choose);
        ui->view->m_color.BG[0] = choose.red();
        ui->view->m_color.BG[1] = choose.green();
        ui->view->m_color.BG[2] = choose.blue();
        ui->view->update();
    }
}

void MainWindow::choose_hint_color()
{
    QColor choose = QColorDialog::getColor(ui->Hint_display->get_color(), nullptr, "Hint");
    if (choose.isValid()) {
        ui->Hint_display->set_color(choose);
        ui->view->m_color.hint[0] = choose.red();
        ui->view->m_color.hint[1] = choose.green();
        ui->view->m_color.hint[2] = choose.blue();
        ui->view->update();
    }
}

void MainWindow::choose_dark_color()
{
    QColor choose = QColorDialog::getColor(ui->Dark_display->get_color(), nullptr, "Dark Disk");
    if (choose.isValid()) {
        ui->Dark_display->set_color(choose);
        ui->view->m_color.dark[0] = choose.red();
        ui->view->m_color.dark[1] = choose.green();
        ui->view->m_color.dark[2] = choose.blue();
        ui->view->update();
    }
}

void MainWindow::choose_light_color()
{
    QColor choose = QColorDialog::getColor(ui->Light_display->get_color(), nullptr, "Light Disk");
    if (choose.isValid()) {
        ui->Light_display->set_color(choose);
        ui->view->m_color.light[0] = choose.red();
        ui->view->m_color.light[1] = choose.green();
        ui->view->m_color.light[2] = choose.blue();
        ui->view->update();
    }
}

void MainWindow::choose_border_color()
{
    QColor choose = QColorDialog::getColor(ui->Border_display->get_color(), nullptr, "Border");
    if (choose.isValid()) {
        ui->Border_display->set_color(choose);
        ui->view->m_color.border[0] = choose.red();
        ui->view->m_color.border[1] = choose.green();
        ui->view->m_color.border[2] = choose.blue();
        ui->view->update();
    }
}
