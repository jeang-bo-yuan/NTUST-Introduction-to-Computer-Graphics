/**
 * @file Box_VAO.h
 * @brief ø�����ɥΪ�VAO
 */
#ifndef BOX_VAO_H
#define BOX_VAO_H

#include "VAO_Interface.h"


/// ø�����ɥΪ�VAO
class Box_VAO : public VAO_Interface
{
private:
    GLuint m_position_vbo;
    GLuint m_ebo;
    GLsizei m_ebo_len;

public:
    /// @brief constructor�A�ͦ�VBO�BEBO
    /// @param size - ������j�p
    Box_VAO(GLfloat size);

    /// �I�s glDeleteBuffers
    virtual ~Box_VAO();

    /// �e�X�ӡA������d��G(-size, -size, -size) ~ (size, size, size)
    void draw() override;
};

#endif // BOX_VAO_H
