/**
 * @file ColorDisplay.h
 * @brief 顯示顏色
 */
#ifndef COLORDISPLAY_H
#define COLORDISPLAY_H


#include <QWidget>
#include <QColor>
#include <stdint.h>

/**
 * @brief 顯示顏色
 */
class ColorDisplay : public QWidget
{
    Q_OBJECT

    QColor m_color;

protected:
    void paintEvent(QPaintEvent*) override;

public:
    /**
     * @brief constructor
     * @param parent - parent widget
     */
    explicit ColorDisplay(QWidget *parent = nullptr);

    /**
     * @brief 取得正在顯示的顏色
     * @return 顏色
     */
    const QColor& get_color() const {
        return m_color;
    }

public slots:
    /**
     * @brief 設定顏色
     * @param c - color
     */
    void set_color(QColor c) {
        m_color = c;
        this->update();
    }

    /**
     * @brief 從陣列設定顏色
     * @param arr - uint8_t的陣列，內含`{R, G, B}`
     */
    void set_color_ub(uint8_t* arr) {
        m_color = QColor(arr[0], arr[1], arr[2]);
        this->update();
    }
};

#endif // COLORDISPLAY_H
