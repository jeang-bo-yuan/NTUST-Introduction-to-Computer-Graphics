/**
 * @file VAO_Interface.h
 * @brief VAO���󪺰򥻤���
 */
#ifndef VAO_INTERFACE_H
#define VAO_INTERFACE_H

#include <glad/glad.h>

/**
 * @brief VAO���󪺰򥻤���
 */
class VAO_Interface {
protected:
    /// ��`glGenVertexArrays`�ͦ���id
    GLuint m_VAO_id;

public:
    /// constructor�A�I�s`glGenVertexArrays`
    VAO_Interface() {
        glGenVertexArrays(1, &m_VAO_id);
    }

    /// deleted copy constructor
    VAO_Interface(const VAO_Interface&)=delete;

    /// deleted move constructor
    VAO_Interface(VAO_Interface&&)=delete;

    /// destructor�A�I�s`glDeleteVertexArrays`
    virtual ~VAO_Interface() {
        glDeleteVertexArrays(1, &m_VAO_id);
    }

    /// �Ѥl���O��@�Aø�s�ϧ�
    virtual void draw()=0;
};

#endif // VAO_INTERFACE_H
