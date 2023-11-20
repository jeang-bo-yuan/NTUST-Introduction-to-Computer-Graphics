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
     * @brief 建構子，參數為圖片檔的路徑
     * @param pX - positive X
     * @param nX - negative X
     * @param pY - positive Y
     * @param nY - negative Y
     * @param pZ - positive Z
     * @param nZ - negative Z
     * @throw std::invalid_argument - 若無法開啟圖片
     */
    qtTextureCubeMap(const std::string& pX, const std::string& nX, const std::string& pY, const std::string& nY, const std::string& pZ, const std::string& nZ);

    /// 解構子，呼叫 glDeleteTextures
    ~qtTextureCubeMap();

    /**
     * @brief 綁定到特定的 samplerCube
     * @param sampler
     */
    void bind_to(GLuint sampler);

    /**
     * @brief 從特定的 sampler 解除綁定
     * @param sampler
     */
    void unbind_from(GLuint sampler);
};

#endif // QTTEXTURECUBEMAP_H
