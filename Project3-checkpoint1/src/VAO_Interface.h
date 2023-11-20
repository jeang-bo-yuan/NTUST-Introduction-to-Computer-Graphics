/**
 * @file VAO_Interface.h
 * @brief VAO物件的基本介面
 */
#ifndef VAO_INTERFACE_H
#define VAO_INTERFACE_H

#include <glad/glad.h>

/**
 * @brief VAO物件的基本介面
 */
class VAO_Interface {
protected:
    /// 由`glGenVertexArrays`生成的id
    GLuint m_VAO_id;

public:
    /// constructor，呼叫`glGenVertexArrays`
    VAO_Interface() {
        glGenVertexArrays(1, &m_VAO_id);
    }

    /// deleted copy constructor
    VAO_Interface(const VAO_Interface&)=delete;

    /// deleted move constructor
    VAO_Interface(VAO_Interface&&)=delete;

    /// destructor，呼叫`glDeleteVertexArrays`
    virtual ~VAO_Interface() {
        glDeleteVertexArrays(1, &m_VAO_id);
    }

    /// 由子類別實作，繪製圖形
    virtual void draw()=0;
};

#endif // VAO_INTERFACE_H
