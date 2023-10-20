/**
 * @file GameView.h
 * @brief 顯示棋盤的Wdiget
 */
#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QOpenGLWidget>
#include <memory>
#include "Game.h"


/// 顯示棋盤的Widget
class GameView : public QOpenGLWidget
{
    Q_OBJECT

private:
    /// Board的邊長
    float m_board_size;
    /// 一個格子的邊長
    float m_slot_size;
    /// 水平的margin
    float m_marginH;
    /// 鉛直的margin
    float m_marginV;
    /// points to the Game object
    std::shared_ptr<Game> m_game_ptr;

public:
    /// @name Vertex Array
    /// @{

    /// 包含棋盤格的所有邊線，共36組vertex，每組含(x, y)，每2組成一線
    static const GLint border_vertex_arr[72];

    /// unit_circle_arr 回傳的陣列有幾組vertex
    static constexpr GLsizei UNIT_CIRCLE_VERTEX_NUM = 40;
    /// @brief 取得單位圓的Vertex Array（半徑=1，以原點為圓心）
    /// @return 共 \ref UNIT_CIRCLE_VERTEX_NUM 組vertex，每組含(x, y)
    static const GLfloat* unit_circle_arr(void);

    /// @}
public:
    /**
     * @brief constructor
     * @param parent - parent widget
     */
    explicit GameView(QWidget* parent = nullptr);

    /**
     * @brief 設置Game Object
     * @param ptr - game object的pointer（std::shared_ptr）
     */
    void set_game(std::shared_ptr<Game> ptr) {
        m_game_ptr = ptr;
    }

protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent*) override;

signals:
    /**
     * @brief 若滑鼠點擊事件成功，則emit
     */
    void clicked_success();
};

#endif // GAMEVIEW_H
