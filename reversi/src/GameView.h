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

    /// 是否要繪製提示
    bool m_draw_hint;

public:
    /// @brief Color
    /// @details 改完後要`update()`才會生效
    struct {
        /// 背景色
        uint8_t BG[3];

        /// 提示的顏色
        uint8_t hint[3];

        /// 黑棋的顏色
        uint8_t dark[3];

        /// 白棋的顏色
        uint8_t light[3];

        /// 邊界的顏色
        uint8_t border[3];
    } m_color;

public:
    /// @name Vertex Array
    /// @{

    /// 包含棋盤格的所有邊線，共36組vertex，每組含(x, y)，每2組成一線
    static const GLint border_vertex_arr[72];

    /// unit_circle_arr 回傳的陣列有幾組vertex
    static constexpr GLsizei UNIT_CIRCLE_VERTEX_NUM = 90;
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

    /**
     * @brief 處理滑鼠點擊事件
     * @details 若點在格子上而且放置棋子成功，則`repaint()`並`emit clicked_success()`
     */
    void mousePressEvent(QMouseEvent*) override;

signals:
    /**
     * @brief 若滑鼠點擊事件成功，則emit
     */
    void clicked_success();

public slots:
    /**
     * @brief 切換提示的顯示
     * @details 剛建構時為開啟
     * @param on - 是否開啟
     */
    void toggle_hint(bool on);

    /**
     * @brief 將m_color設成經典樣式，並立即update
     */
    void use_classic_theme();

    /**
     * @brief 使用主題2，並立即update
     */
    void use_theme2();
};

#endif // GAMEVIEW_H
