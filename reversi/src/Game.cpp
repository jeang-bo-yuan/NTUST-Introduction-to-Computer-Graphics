
#include "Game.h"
#include <string.h>
#include <QPoint>
#include <QtGlobal>
#include <QDebug>

size_t Game::calc_can_click()
{
    // 重置
    memset(m_can_click, false, sizeof(m_can_click));

    // 目前行動方的棋子
    Disk current = m_is_dark_turn ? Disk::Dark : Disk::Light;
    // 記錄可點擊的數量
    size_t moves = 0;

    // 從pos沿著dir，找有沒有可以點的    Note: QPoint(col, row)
    auto find_can_click = [current, this](QPoint pos, QPoint dir) -> QPoint {
        int r = pos.y() + dir.y(); // 馬上沿著dir走
        int c = pos.x() + dir.x();
        // 是否有碰到對方的棋子
        bool meet_opponent = false;
        // while r, c在邊界內
        while (0 <= r && r < 8 && 0 <= c && c < 8) {
            // 先碰到我方棋子 -> 這方向沒有可點的
            if (this->m_board[r][c] == current)
                break;
            else if (this->m_board[r][c] == Disk::None) {
                // 碰到空格前要先碰到對方的棋子才算可以點
                if (meet_opponent)
                    return QPoint(c, r);
                else
                    break;
            }

            meet_opponent = true;
            r += dir.y();
            c += dir.x();
        }
        // 回傳(-1, -1)代表沒找到可以點的位置
        return QPoint(-1, -1);
    };

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // 若是行動方的棋子，則找看看有沒有可以點的
            if (m_board[row][col] == current) {
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <=1; ++dy) {
                        if (dx == 0 && dy == 0) continue;

                        QPoint result = find_can_click(QPoint(col, row), QPoint(dx, dy));
                        // not found
                        if (result == QPoint(-1, -1)) continue;

                        ++moves;
                        m_can_click[result.y()][result.x()] = true;
                    }
                }
            }
        }
    }

    return moves;
}

void Game::reverse_disk(QPoint pos, QPoint dir)
{
    Q_ASSERT(m_board[pos.y()][pos.x()] != Disk::None);

    int r = pos.y() + dir.y(); // 馬上沿著dir走
    int c = pos.x() + dir.x();

    // pos住dir方向走，發現的第一個同色棋子
    QPoint another(-1, -1);
    while (0 <= r && r < 8 && 0 <= c && c < 8) {
        // 夾著空格就不轉
        if (m_board[r][c] == Disk::None)
            return;
        // 夾著對方的棋子，繼續找找看
        else if (m_board[r][c] != m_board[pos.y()][pos.x()]) {
            r += dir.y();
            c += dir.x();
        }
        // 找到同色
        else {
            another.setY(r);
            another.setX(c);
            break;
        }
    }
    // not found
    if (another == QPoint(-1, -1)) return;

    // 翻轉成
    Disk reverse_to = m_board[pos.y()][pos.x()];
    // 記錄翻轉了幾個
    uint8_t count = 0;
    r = pos.y() + dir.y();
    c = pos.x() + dir.x();
    // 將pos和another間的棋子翻轉
    while (r != another.y() || c != another.x()) { // while (c, r) != another
        m_board[r][c] = reverse_to;
        ++count;

        r += dir.y();
        c += dir.x();
    }

    // 更新黑、白棋的數量
    if (reverse_to == Disk::Dark) {
        m_dark_num += count;
        m_light_num -= count;
    }
    else {
        m_light_num += count;
        m_dark_num -= count;
    }
}

void Game::next_round(bool dont_change_side)
{
    m_state = State::Playing; // 先假設下個回合仍是Playing

    if (dont_change_side == false)
        m_is_dark_turn = !m_is_dark_turn; // 換人

    size_t num = calc_can_click();
    if (num == 0) {
        // 沒辦法動則直接換對方
        m_is_dark_turn = !m_is_dark_turn;
        num = calc_can_click();

        // 若還是不能動則結束
        if (num == 0) {
            m_state = m_dark_num > m_light_num ? State::Dark_Win :
                      m_dark_num < m_light_num ? State::Light_Win :
                                                 State::Draw;
        }
    }
}

// Game Public Method //////////////////////////////////////////////////////////////////////////

///
Game::Game()
{
    this->reset();
}

bool Game::click(int row, int col)
{
    // 若遊戲不在進行中
    if (m_state != State::Playing) return false;

    // 若不能點
    if (!m_can_click[row][col]) return false;

    // 將上回合的情形存入stack
    m_stack.push(m_board, m_is_dark_turn);

    // 放上棋子
    if (m_is_dark_turn) {
        m_board[row][col] = Disk::Dark;
        ++m_dark_num;
    }
    else {
        m_board[row][col] = Disk::Light;
        ++m_light_num;
    }

    // 翻轉棋子
    QPoint pos(col, row);
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;

            this->reverse_disk(pos, QPoint(dx, dy));
        }
    }

    // 進入下一回合+計算棋步
    next_round();

    return true;
}

void Game::reset()
{
    m_stack.clear();

    memset(m_board, 0, sizeof(m_board));
    m_board[3][3] = Disk::Light;
    m_board[4][4] = Disk::Light;
    m_board[3][4] = Disk::Dark;
    m_board[4][3] = Disk::Dark;

    m_dark_num = 2;
    m_light_num = 2;

    m_is_dark_turn = true;

    m_state = State::Playing;
    calc_can_click();
}

void Game::undo()
{
    qDebug() << "Undo:";
    if (m_stack.empty()) return;

    if (m_stack.can_restore() == false) {
        m_stack.push(m_board, m_is_dark_turn);
        m_stack.pop();
    }

    m_stack.peek(m_board, m_is_dark_turn);
    m_stack.pop();

    // 重新計算黑、白的數量
    m_dark_num = 0;
    m_light_num = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            switch (m_board[r][c]) {
            case Disk::None:
                break;
            case Disk::Dark:
                ++m_dark_num;
                break;
            case Disk::Light:
                ++m_light_num;
                break;
            }
        }
    }

    next_round(true);
}

void Game::redo()
{
    qDebug() << "Redo:";
    if (m_stack.can_restore() == false) return;

    m_stack.restore();
    m_stack.restore();
    m_stack.peek(m_board, m_is_dark_turn);
    m_stack.pop();

    // 重新計算黑、白的數量
    m_dark_num = 0;
    m_light_num = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            switch (m_board[r][c]) {
            case Disk::None:
                break;
            case Disk::Dark:
                ++m_dark_num;
                break;
            case Disk::Light:
                ++m_light_num;
                break;
            }
        }
    }

    next_round(true);
}


// GamingStack //////////////////////////////////////////////////////////////////////////////////////////////

///
void Game::GamingStack::push(const Disk board[][8], bool is_dark)
{
    Game::GamingStack::Data data;
    memcpy(&data.m_board, &board[0][0], sizeof(Data::m_board));
    data.m_is_dark_turn = is_dark;
    m_stack.resize(m_size);
    m_stack.emplace_back(data);
    ++m_size;

    qDebug() << "Push index: " << m_size - 1;
}

void Game::GamingStack::pop()
{
    if(this->empty()) return;

    --m_size;

    qDebug() << "Pop index: " << m_size;
}

void Game::GamingStack::restore()
{
    if (this->can_restore() == false) return;

    ++m_size;

    qDebug() << "Restore index: " << m_size - 1;
}

void Game::GamingStack::peek(Disk board[][8], bool &is_dark)
{
    if (this->empty()) return;

    memcpy(&board[0][0], &m_stack[m_size - 1].m_board, sizeof(Data::m_board));
    is_dark = m_stack[m_size - 1].m_is_dark_turn;
    qDebug() << "Peek index: " << m_size - 1;
}