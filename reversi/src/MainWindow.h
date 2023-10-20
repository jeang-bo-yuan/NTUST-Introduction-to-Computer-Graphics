/**
 * @file MainWindow.h
 * @brief 主視窗
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Game.h"
#include <memory>

namespace Ui {
class MainWindow;
}

/**
 * @brief 主視窗
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    /// Ui 排版
    Ui::MainWindow *ui;

    /// points to game object
    std::shared_ptr<Game> m_game_ptr;

public:
    /**
     * @brief 建構子
     * @param parent - 父Widget
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief 解構子
     */
    ~MainWindow();

public slots:
    /**
     * @brief 更新顯示的資訊
     */
    void update_info();

    /**
     * @brief 重新開始
     */
    void restart();
};

#endif // MAINWINDOW_H
