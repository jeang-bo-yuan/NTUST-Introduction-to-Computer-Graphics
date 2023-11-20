
#include "Box_VAO.h"

Box_VAO::Box_VAO(GLfloat size)
{
    GLfloat positions[] = {
        -size, -size, -size,
        -size, -size, +size,
        -size, +size, -size,
        -size, +size, +size,
        +size, -size, -size,
        +size, -size, +size,
        +size, +size, -size,
        +size, +size, +size
    };
    GLuint elements[] = {
        0, 1, 3, 2,
        0, 4, 6, 2,
        0, 1, 5, 4,
        7, 6, 2, 3,
        7, 5, 4, 6,
        7, 5, 1, 3
    };
    m_ebo_len = sizeof(elements) / sizeof(GLuint);

    glGenBuffers(1, &m_position_vbo);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(m_VAO_id);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Box_VAO::~Box_VAO()
{
    glDeleteBuffers(1, &m_position_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Box_VAO::draw()
{
    glBindVertexArray(m_VAO_id);
    glDrawElements(GL_QUADS, m_ebo_len, GL_UNSIGNED_INT, (void*)0);
}
