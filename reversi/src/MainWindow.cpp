#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_game_ptr(std::make_shared<Game>())
{
    ui->setupUi(this);
    ui->view->set_game(m_game_ptr);
    this->update_info();

    // 遊戲介面
    connect(ui->view, &GameView::clicked_success, this, &MainWindow::update_info);
    connect(ui->buttonUndo, &QPushButton::clicked, this, &MainWindow::undo);
    connect(ui->buttonRedo, &QPushButton::clicked, this, &MainWindow::redo);

    // 上方的menu
    ui->menuFrame->layout()->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    connect(ui->buttonRestart, &QPushButton::clicked, this, &MainWindow::restart);
    connect(ui->buttonToggleHint, &QPushButton::toggled, ui->view, &GameView::toggle_hint);
    connect(ui->buttonPalette, &QPushButton::toggled, this, &MainWindow::toggle_palette);
    connect(ui->buttonTutorial, &QPushButton::clicked, this, [this](){ this->toggle_tutorial(true); });
    connect(ui->buttonTutorialClose, &QPushButton::clicked, this, [this]{ this->toggle_tutorial(false); });

    // 初始化Palette
    sync_color_display();

    // 連接Palette的按鈕
    connect(ui->buttonSelectBGColor, &QPushButton::clicked, this, &MainWindow::choose_BG_color);
    connect(ui->buttonSelectHintColor, &QPushButton::clicked, this, &MainWindow::choose_hint_color);
    connect(ui->buttonSelectDarkColor, &QPushButton::clicked, this, &MainWindow::choose_dark_color);
    connect(ui->buttonSelectLightColor, &QPushButton::clicked, this, &MainWindow::choose_light_color);
    connect(ui->buttonSelectBorderColor, &QPushButton::clicked, this, &MainWindow::choose_border_color);
    // theme
    connect(ui->buttonClassicTheme, &QPushButton::clicked, this, &MainWindow::classic_theme);
    connect(ui->buttonTheme2, &QPushButton::clicked, this, &MainWindow::theme2);
    // file
    connect(ui->buttonSavePalette, &QPushButton::clicked, this, &MainWindow::save_palette);
    connect(ui->buttonLoadPalette, &QPushButton::clicked, this, &MainWindow::load_palette);
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

void MainWindow::toggle_tutorial(bool open)
{
    if (open) {
        ui->stackMain->setCurrentWidget(ui->pageTutorial);
        ui->menuFrame->hide();
    }
    else {
        ui->stackMain->setCurrentWidget(ui->pagePlaying);
        ui->menuFrame->show();
    }
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

void MainWindow::classic_theme()
{
    ui->view->use_classic_theme();
    sync_color_display();
}

void MainWindow::theme2()
{
    ui->view->use_theme2();
    sync_color_display();
}

void MainWindow::save_palette()
{
    QString file = QFileDialog::getSaveFileName(this, "儲存配色", qApp->applicationDirPath(), "Binary (*.bin)");

    if (file.isEmpty()) return;

    std::ofstream outF(file.toStdString());

    if (!outF.is_open()) {
        QMessageBox::warning(this, "Save Failed", QString("無法開啟檔案並儲存：").append(file));
        return;
    }

    outF.write((char*)&ui->view->m_color, sizeof(ui->view->m_color));
}

void MainWindow::load_palette()
{
    QString file = QFileDialog::getOpenFileName(this, "載入配色", qApp->applicationDirPath(), "Binary (*.bin)");

    if (file.isEmpty()) return;

    std::ifstream inF(file.toStdString());

    if (!inF.is_open()) {
        QMessageBox::warning(this, "Load Failed", QString("無法開啟檔案並載入：").append(file));
        return;
    }

    inF.read((char*)&ui->view->m_color, sizeof(ui->view->m_color));
    ui->view->update();
    sync_color_display();
}

void MainWindow::undo()
{
    m_game_ptr->undo();
    ui->view->update();
    this->update_info();
}

void MainWindow::redo()
{
    m_game_ptr->redo();
    ui->view->update();
    this->update_info();
}

void MainWindow::sync_color_display()
{
    ui->BG_display->set_color_ub(ui->view->m_color.BG);
    ui->Hint_display->set_color_ub(ui->view->m_color.hint);
    ui->Dark_display->set_color_ub(ui->view->m_color.dark);
    ui->Light_display->set_color_ub(ui->view->m_color.light);
    ui->Border_display->set_color_ub(ui->view->m_color.border);
}
