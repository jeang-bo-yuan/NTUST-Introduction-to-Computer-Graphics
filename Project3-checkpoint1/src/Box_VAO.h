/**
 * @file Box_VAO.h
 * @brief 繪制方塊時用的VAO
 */
#ifndef BOX_VAO_H
#define BOX_VAO_H

#include "VAO_Interface.h"


/// 繪制方塊時用的VAO
class Box_VAO : public VAO_Interface
{
private:
    GLuint m_position_vbo;
    GLuint m_ebo;
    GLsizei m_ebo_len;

public:
    /// @brief constructor，生成VBO、EBO
    /// @param size - 方塊的大小
    Box_VAO(GLfloat size);

    /// 呼叫 glDeleteBuffers
    virtual ~Box_VAO();

    /// 畫出來，方塊的範圍：(-size, -size, -size) ~ (size, size, size)
    void draw() override;
};

#endif // BOX_VAO_H
