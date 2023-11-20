
#include "Wave_VAO.h"
#include <glm/vec3.hpp>
#include <vector>
#include <assert.h>

Wave_VAO::Wave_VAO(GLfloat size)
    : VAO_Interface(), m_vbo_position(0), m_ebo(0), m_num_of_elements(0)
{
    const float delta = 1;
    // 畫在(-size, 0, -size) ~ (size, 0, size)
    // 每 1/16 取一個點
    std::vector<glm::vec3> point_arr;
    GLuint width = 0;
    point_arr.reserve((32 * size + 1) * (32 * size + 1));
    {
        for (float z = -size; z <= size; z += delta) {
            for (float x = -size; x <= size; x += delta) {
                point_arr.emplace_back(x, 0, z);
            }
            ++width;
        }
    }
    assert(point_arr.size() == (width * width));

    // 生成element array
    std::vector<GLuint> elem_arr;
    elem_arr.reserve((32 * size) * (32 * size) * 2 * 3);
    {
        auto to_index = [width](GLuint r, GLuint c)->GLuint {
            return r * width + c;
        };

        for (GLuint r = 0; r < width - 1; ++r) {
            for (GLuint c = 0; c < width - 1; ++c) {
                elem_arr.push_back(to_index(r, c));
                elem_arr.push_back(to_index(r + 1, c));
                elem_arr.push_back(to_index(r, c + 1));

                elem_arr.push_back(to_index(r, c + 1));
                elem_arr.push_back(to_index(r + 1, c));
                elem_arr.push_back(to_index(r + 1, c + 1));
            }
        }
    }
    m_num_of_elements = elem_arr.size();

    glGenBuffers(1, &m_vbo_position);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(m_VAO_id);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_position);
    glBufferData(GL_ARRAY_BUFFER, point_arr.size() * sizeof(glm::vec3), point_arr.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elem_arr.size() * sizeof(GLuint), elem_arr.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Wave_VAO::~Wave_VAO()
{
    glDeleteBuffers(1, &m_vbo_position);
    glDeleteBuffers(1, &m_ebo);
}

void Wave_VAO::draw()
{
    glBindVertexArray(m_VAO_id);
    glDrawElements(GL_TRIANGLES, m_num_of_elements, GL_UNSIGNED_INT, (void*)0);
}

