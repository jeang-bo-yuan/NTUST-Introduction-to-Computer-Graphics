/**
 * @file Game.h
 * @brief 代表遊戲的class
 */
#ifndef GAME_H
#define GAME_H

#include <string>
#include <stdint.h>

class QPoint;

/// 代表遊戲的class
class Game
{
public:
    /// 棋子的種類
    enum class Disk : char {
        None = 0,     //!< 沒有棋子
        Dark = 'D',   //!< 黑色的棋子（先手）
        Light = 'L',  //!< 白色的棋子（後手）
    };

    /// 代表遊玩狀態
    enum class State : char {
        Playing = 'P',   //!< 遊戲進行中
        Dark_Win = 'W',  //!< 黑色贏了
        Light_Win = 'w', //!< 白色贏了
        Draw = 'D'       //!< 平手
    };

private:
    /// 棋盤
    Disk m_board[8][8];

    /// 記錄可不可以放棋子
    bool m_can_click[8][8];

    /// 是否為黑色的回合
    bool m_is_dark_turn;

    /// 遊玩狀態
    State m_state;

    /// 盤面上的黑棋個數
    uint8_t m_dark_num;

    /// 盤面上的白棋個數
    uint8_t m_light_num;

private:
    /**
     * @brief 計算目前的行動方可以點的位置
     * @return 有幾個格子可以點，若`m_state != State::Playing`會回傳`0`
     * @post m_can_click會重置並更新
     */
    size_t calc_can_click();

    /**
     * @brief 翻轉棋子，若pos往dir的方向為「pos上的棋子」夾住「對方的棋子」，則將被夾住的翻轉
     * @param pos - 啟始點 (col, row)
     * @param dir - 方向 (dx, dy)
     * @pre pos上不能是Disk::None
     */
    void reverse_disk(QPoint pos, QPoint dir);

public:
    /**
     * @brief constructor，建構後立即呼叫Game::reset
     */
    Game();

    /// @name Access The Board
    /// Note: row和col都要在`[0, 8)`間，函數不會做邊界檢查
    /// @{

    /**
     * @brief 確認能不能點
     * @param row - 列
     * @param col - 欄
     * @return 若可以點，回傳`true`；否則回傳`false`
     */
    bool can_click(int row, int col) const {
        return m_can_click[row][col];
    }

    /**
     * @brief 點擊，若成功則放上棋子並進入下一回合
     * @param row - 列
     * @param col - 欄
     * @return 放置成功則回傳`true`，否則回傳`false`
     */
    bool click(int row, int col);

    /**
     * @brief 取得盤面上的棋子
     * @param row - 列
     * @param col - 欄
     * @return 棋子的種類 Game::Disk
     */
    Disk get_disk(int row, int col) const {
        return m_board[row][col];
    }

    /// @}

    /// @name Game State Management
    /// @{

    /**
     * @brief 取得黑棋數
     * @return 黑棋數
     */
    int get_dark_num() const {
        return m_dark_num;
    }

    /**
     * @brief 取得白棋數
     * @return 白棋數
     */
    int get_light_num() const {
        return m_light_num;
    }

    /**
     * @brief 取得遊戲的狀態（state）
     * @return Game::State 中的其中一個
     */
    State get_state() const {
        return m_state;
    }

    /**
     * @brief 確認是不是黑色的回合
     * @return 如果是黑色的回合，回傳`true`；否則回傳`false`
     */
    bool is_dark_turn() const {
        return m_is_dark_turn;
    }

    /**
     * @brief 重設棋盤和遊戲狀態（即重新開始）
     * @details 設成黑色的回合，狀態（state）設成 State::Playing ，且棋盤只有中間四格有棋
     * ```
     * --- ---- ---
     * --- 白 黑 ---
     * --- 黑 白 ---
     * --- ---- ---
     * ```
     */
    void reset();

    /// @}
};

#endif // GAME_H
