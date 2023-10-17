/**
 * @file Game.h
 * @brief 代表遊戲的class
 */
#ifndef GAME_H
#define GAME_H

#include <string>

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

    /// 是否為黑色的回合
    bool m_is_dark_turn;

    /// 遊玩狀態
    State m_state;

public:
    /**
     * @brief constructor，建構後立即呼叫Game::reset
     */
    Game();

    /// @name Access The Board
    /// Note: row和col都要在`[0, 8)`間，函數不會做邊界檢查
    /// @{

    /**
     * @brief 取得盤面上的棋子
     * @param row - 列
     * @param col - 欄
     * @return 棋子的種類 Game::Disk
     */
    Disk get_disk(size_t row, size_t col) const {
        return m_board[row][col];
    }

    /**
     * @brief 點擊，若成功則放上棋子並進入下一回合
     * @param row - 列
     * @param col - 欄
     * @return 放置成功則回傳`true`，否則回傳`false`
     * @todo
     * 1. 確認能不能放
     * 2. 進入下一回合時，要計算能走的棋步
     */
    bool click(size_t row, size_t col);

    /// @}

    /// @name Game State Management
    /// @{

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
