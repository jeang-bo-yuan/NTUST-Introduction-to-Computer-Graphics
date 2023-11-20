/**
 * @file Wave_VAO.h
 * @brief �@�ӭ���VAO
 */
#ifndef WAVE_VAO_H
#define WAVE_VAO_H

#include "VAO_Interface.h"


/**
 * @brief ø�ssin wave�ɨϥΪ�VAO
 * @details ���u�O�@�ӥ����A�n�f�tShader���নsin wave
 */
class Wave_VAO : public VAO_Interface {
private:
    /// ���I�y�Ъ�VBO
    GLuint m_vbo_position;
    /// EBO
    GLuint m_ebo;
    /// EBO���X�Ӥ���
    GLuint m_num_of_elements;

public:
    /// Constructor
    /// @param size - ���i���d��G(-size, 0, -size) ~ (size, 0, size)
    Wave_VAO(GLfloat size);

    /// Destructor
    ~Wave_VAO();

    /// �b(-1, 0, -1) ~ (1, 0, 1)�e�X�@�ӥѳ\�h�T���κc�������A�n�f�tShader�নsine wave
    void draw() override;
};

#endif // WAVE_VAO_H
