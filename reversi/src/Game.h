/**
 * @file Game.h
 * @brief 儲存遊戲的class
 */
#ifndef GAME_H
#define GAME_H

#include <string>


class Game
{
public:
    enum class Disk {
        None = 0,   //!< 沒有棋子
        Dark = 1,   //!< 黑色的棋子（先手）
        Light = 2,  //!< 白色的棋子（後手）
    };

private:
    Disk m_board[8][8];

public:
    Game();

    void restart();

    void writeToFile(const std::string& file_name);

    void readFromFile(const std::string& file_name);
};

#endif // GAME_H
