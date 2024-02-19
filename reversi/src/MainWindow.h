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

    /**
     * @brief 切換palette的顯示
     * @param open - 是否開啟
     */
    void toggle_palette(bool open);

    /**
     * @brief 切換pageTutorial
     * @param open - 是否開啟
     */
    void toggle_tutorial(bool open);

    /// 顯示Color Dialog來選擇顏色
    void choose_BG_color();

    /// 顯示Color Dialog來選擇顏色
    void choose_hint_color();

    /// 顯示Color Dialog來選擇顏色
    void choose_dark_color();

    /// 顯示Color Dialog來選擇顏色
    void choose_light_color();

    /// 顯示Color Dialog來選擇顏色
    void choose_border_color();

    /// 將棋盤切換成經典樣式
    void classic_theme();

    /// theme2
    void theme2();

    /// 儲存配色
    void save_palette();

    /// 載入配色
    void load_palette();

private:
    /// 將color display顯示的顏色和ui->view實際的顏色同步
    void sync_color_display();
};

#endif // MAINWINDOW_H
