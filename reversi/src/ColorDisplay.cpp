
#include "ColorDisplay.h"
#include <QPainter>

void ColorDisplay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(m_color);
    painter.drawRect(0, 0, width(), height());
}

ColorDisplay::ColorDisplay(QWidget *parent)
    : QWidget{parent}, m_color(0, 0, 0)
{

}

