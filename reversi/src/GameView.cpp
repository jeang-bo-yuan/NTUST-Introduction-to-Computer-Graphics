
#include "GameView.h"
#include <gl/gl.h>
#include <algorithm>
#include <cmath>
#include <QMouseEvent>
#include <iostream>

#define DISK_RADIUS 0.4f

const GLint GameView::border_vertex_arr[72] = {
    // 橫的
    0, 0, 8, 0,
    0, 1, 8, 1,
    0, 2, 8, 2,
    0, 3, 8, 3,
    0, 4, 8, 4,
    0, 5, 8, 5,
    0, 6, 8, 6,
    0, 7, 8, 7,
    0, 8, 8, 8,
    // 直的
    0, 0, 0, 8,
    1, 0, 1, 8,
    2, 0, 2, 8,
    3, 0, 3, 8,
    4, 0, 4, 8,
    5, 0, 5, 8,
    6, 0, 6, 8,
    7, 0, 7, 8,
    8, 0, 8, 8
};

const GLfloat *GameView::unit_circle_arr() {
    static bool is_initialized = false;
    static GLfloat arr[UNIT_CIRCLE_VERTEX_NUM][2];

    if (is_initialized) return &arr[0][0];

    float delta_theta = 2 * /*PI*/3.14159265358979323846f / (float)UNIT_CIRCLE_VERTEX_NUM;
    float theta = 0.f;
    for (size_t i = 0; i < UNIT_CIRCLE_VERTEX_NUM; ++i) {
        arr[i][0] = cosf(theta); // x
        arr[i][1] = sinf(theta); // y

        theta += delta_theta;
    }
    is_initialized = true;

    return &arr[0][0];
}

GameView::GameView(QWidget* parent)
    : QOpenGLWidget(parent)
{
    this->setMinimumSize(200, 200);
}

void GameView::initializeGL() {
    glMatrixMode(GL_PROJECTION);
    // (0,0) .......
    //   |         |
    //   ........(8,8)
    // 為了讓四周的線看起來比較粗，所以稍微向外加大了一點
    glOrtho(-0.02, 8.02, 8.02, -0.02, -1, 1);

    glLineWidth(3);
    glEnableClientState(GL_VERTEX_ARRAY);

    glClearColor(32 / 255.f, 122 / 255.f, 38 / 255.f, 0);
}

void GameView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(m_marginH, m_marginV, m_board_size, m_board_size);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw disk
    if (m_game_ptr) {
        glVertexPointer(2, GL_FLOAT, 0, unit_circle_arr()); // 設置單位圓的vertex array
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (m_game_ptr->can_click(row, col)) {
                    glColor4ub(232, 229, 132, 10);
                    glRecti(col, row, col + 1, row + 1);
                }

                switch(m_game_ptr->get_disk(row, col)) {
                case Game::Disk::None:
                    continue;
                case Game::Disk::Dark:
                    glColor3ub(0, 0, 0);
                    break;
                case Game::Disk::Light:
                    glColor3ub(255, 255, 255);
                    break;
                }

                // draw a circle
                glTranslatef(col + 0.5f, row + 0.5f, 0); // 平移到格子中間
                glScalef(DISK_RADIUS, DISK_RADIUS, 0);  // 設定縮放
                glDrawArrays(GL_POLYGON, 0, UNIT_CIRCLE_VERTEX_NUM);
                glLoadIdentity();
            }
        }
    }

    glColor3ub(0, 0, 0);
    glVertexPointer(2, GL_INT, 0, border_vertex_arr);
    glDrawArrays(GL_LINES, 0, 36);
}

void GameView::resizeGL(int w, int h) {
    m_board_size = std::min(w, h) * 0.8f;
    m_slot_size = m_board_size / 8.f;
    m_marginH = (w - m_board_size) / 2.f;
    m_marginV = (h - m_board_size) / 2.f;
}

void GameView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        float x = event->x() - m_marginH;
        float y = event->y() - m_marginV;
        int row = y / m_slot_size;
        int col = x / m_slot_size;

        if (row < 0 || row >= 8 || col < 0 || col >= 8) {
            return;
        }

        std::cout << "row: " << row << ", col: " << col;

        if (m_game_ptr->click(row, col) == false)
            std::cout << " Click Failed!";

        // 檢查state
        switch (m_game_ptr->get_state()) {
        case Game::State::Draw:
            std::cout << "\nDraw";
            break;
        case Game::State::Playing:
            break;
        case Game::State::Dark_Win:
            std::cout << "\nDark Win";
            break;
        case Game::State::Light_Win:
            std::cout << "\nLight Win";
            break;
        }

        std::cout << std::endl;
        this->update();
    }
}

