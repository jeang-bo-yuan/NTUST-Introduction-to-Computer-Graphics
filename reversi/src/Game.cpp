
#include "Game.h"
#include <string.h>

Game::Game()
{
    this->reset();
}

bool Game::click(size_t row, size_t col)
{
    if (m_is_dark_turn) {
        m_board[row][col] = Disk::Dark;
    }
    else {
        m_board[row][col] = Disk::Light;
    }

    m_is_dark_turn = !m_is_dark_turn;

    return true;
}

void Game::reset()
{
    memset(m_board, 0, sizeof(m_board));
    m_board[3][3] = Disk::Light;
    m_board[4][4] = Disk::Light;
    m_board[3][4] = Disk::Dark;
    m_board[4][3] = Disk::Dark;

    m_is_dark_turn = true;
}

