
#include <glad/gl.h>
#include "GameView.h"
#include <algorithm>
#include <cmath>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>

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
    : QOpenGLWidget(parent), m_draw_hint(true),
    m_color { {32, 122, 38}, {232, 229, 132}, {0, 0, 0}, {255, 255, 255}, {0, 0, 0} }
{
    this->setMinimumSize(200, 200);
}

void GameView::initializeGL() {
    int version = gladLoaderLoadGL();
    if (version == 0) {
        QMessageBox::critical(nullptr, "OpenGL load failed", "Unable to load OpenGL.\nMaybe your GPU doesn't support OpenGL.");
        qApp->exit(1);
    }
    qDebug() << "Load OpenGL" << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version);

    glMatrixMode(GL_PROJECTION);
    // (0, 0) .....
    //   |        |
    //   .......(8,8)
    // 為了讓四周的線看起來比較粗，所以稍微向外加大了一點
    glOrtho(-0.02, 8.02, 8.02, -0.02, -1, 1);

    glLineWidth(3);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void GameView::paintGL() {
    glClearColor(m_color.BG[0] / 255.f, m_color.BG[1] / 255.f, m_color.BG[2] / 255.f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(m_marginH, m_marginV, m_board_size, m_board_size);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw disk
    if (m_game_ptr) {
        glVertexPointer(2, GL_FLOAT, 0, unit_circle_arr()); // 設置單位圓的vertex array
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                if (m_draw_hint && m_game_ptr->can_click(row, col)) {
                    glColor3ubv(m_color.hint);
                    glRecti(col, row, col + 1, row + 1);
                }

                switch(m_game_ptr->get_disk(row, col)) {
                case Game::Disk::None:
                    continue;
                case Game::Disk::Dark:
                    glColor3ubv(m_color.dark);
                    break;
                case Game::Disk::Light:
                    glColor3ubv(m_color.light);
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

    glColor3ubv(m_color.border);
    glVertexPointer(2, GL_INT, 0, border_vertex_arr);
    glDrawArrays(GL_LINES, 0, 36);

    // draw coordinate
    QPainter painter(this);
    painter.setFont(QFont("Cascadia Code", 12, QFont::Bold));
    painter.setPen(QColor(m_color.border[0], m_color.border[1], m_color.border[2]));
    int time = 0;
    // 水平的畫
    for (float x = m_marginH, y = m_marginV; time < 8; x += m_slot_size, ++time) {
        painter.drawText(QRectF(x, y - 20, m_slot_size, 20), Qt::AlignHCenter | Qt::AlignBottom, QString('a' + time));
    }
    time = 0;
    // 垂直的畫
    for (float x = m_marginH, y = m_marginV; time < 8; y += m_slot_size, ++time) {
        painter.drawText(QRectF(x - 20, y, 20, m_slot_size), Qt::AlignVCenter | Qt::AlignRight, QString('1' + time));
    }
}

void GameView::resizeGL(int w, int h) {
    m_board_size = std::min(w, h) * 0.8f;
    m_slot_size = m_board_size / 8.f;
    m_marginH = (w - m_board_size) / 2.f;
    m_marginV = (h - m_board_size) / 2.f;
}

void GameView::mousePressEvent(QMouseEvent* event) {
    if (m_game_ptr == nullptr) return;

    if (event->button() == Qt::LeftButton) {
        float x = event->x() - m_marginH;  // Note: Qt的(x,y)是左上為(0,0)
        float y = event->y() - m_marginV;
        int row = floor(y / m_slot_size);
        int col = floor(x / m_slot_size);

        if (row < 0 || row >= 8 || col < 0 || col >= 8) {
            return;
        }

        qDebug() << "row: " << row << ", col: " << col;

        if (m_game_ptr->click(row, col) == false) {
            qDebug() << " Click Failed!";
            return;
        }

        this->repaint();
        emit clicked_success();
    }
}

void GameView::toggle_hint(bool on) {
    m_draw_hint = on;
    this->repaint();
}

void GameView::use_classic_theme()
{
    m_color = decltype(m_color) {
        {32, 122, 38},      // BG
        {232, 229, 132},    // hint
        {0, 0, 0},          // dark
        {255, 255, 255},    // light
        {0, 0, 0}           // border
    };
    this->update();
}

void GameView::use_theme2()
{
    m_color = decltype(m_color) {
        {14, 54, 17},       // BG
        {170, 0, 0},        // hint
        {0, 0, 0},          // dark
        {227, 227, 227},    // light
        {189, 167, 54}      // border
    };
    this->update();
}
