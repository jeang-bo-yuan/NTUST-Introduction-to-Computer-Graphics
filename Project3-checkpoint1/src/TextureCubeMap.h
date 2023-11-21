/**
 * @file qtTextureCubeMap.h
 * @brief A class that wrap GL_TEXTURE_CUBE_MAP
 */
#ifndef QTTEXTURECUBEMAP_H
#define QTTEXTURECUBEMAP_H

#include <glad/glad.h>
#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>

/// @brief A class that wrap GL_TEXTURE_CUBE_MAP
class qtTextureCubeMap
{
private:
    GLuint m_texture_id;

public:
    /**
     * @brief �غc�l�A�ѼƬ��Ϥ��ɪ����|
     * @param pX - positive X
     * @param nX - negative X
     * @param pY - positive Y
     * @param nY - negative Y
     * @param pZ - positive Z
     * @param nZ - negative Z
     * @throw std::invalid_argument - �Y�L�k�}�ҹϤ�
     */
    qtTextureCubeMap(const std::string& pX, const std::string& nX, const std::string& pY, const std::string& nY, const std::string& pZ, const std::string& nZ);

    /// �Ѻc�l�A�I�s glDeleteTextures
    ~qtTextureCubeMap();

    /**
     * @brief �j�w��S�w�� samplerCube
     * @param sampler
     */
    void bind_to(GLuint sampler);

    /**
     * @brief �q�S�w�� sampler �Ѱ��j�w
     * @param sampler
     */
    void unbind_from(GLuint sampler);
};

#endif // QTTEXTURECUBEMAP_H
