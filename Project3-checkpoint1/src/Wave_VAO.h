/**
 * @file Wave_VAO.h
 * @brief 一個面的VAO
 */
#ifndef WAVE_VAO_H
#define WAVE_VAO_H

#include "VAO_Interface.h"


/**
 * @brief 繪製sin wave時使用的VAO
 * @details 其實只是一個平面，要搭配Shader來轉成sin wave
 */
class Wave_VAO : public VAO_Interface {
private:
    /// 頂點座標的VBO
    GLuint m_vbo_position;
    /// EBO
    GLuint m_ebo;
    /// EBO有幾個元素
    GLuint m_num_of_elements;

public:
    /// Constructor
    /// @param size - 水波的範圍：(-size, 0, -size) ~ (size, 0, size)
    Wave_VAO(GLfloat size);

    /// Destructor
    ~Wave_VAO();

    /// 在(-1, 0, -1) ~ (1, 0, 1)畫出一個由許多三角形構成的面，要搭配Shader轉成sine wave
    void draw() override;
};

#endif // WAVE_VAO_H
