
#include "TextureCubeMap.h"

qtTextureCubeMap::qtTextureCubeMap(const std::string &pX, const std::string &nX,
                                   const std::string &pY, const std::string &nY,
                                   const std::string &pZ, const std::string &nZ)
{
    GLint old_cube_map;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &old_cube_map);

    // 產生texture，並綁定
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);

    cv::Mat each_face[6] = {
        cv::imread(pX.c_str(), cv::IMREAD_COLOR), cv::imread(nX.c_str(), cv::IMREAD_COLOR),
        cv::imread(pY.c_str(), cv::IMREAD_COLOR), cv::imread(nY.c_str(), cv::IMREAD_COLOR),
        cv::imread(pZ.c_str(), cv::IMREAD_COLOR), cv::imread(nZ.c_str(), cv::IMREAD_COLOR)
    };
    constexpr GLenum face_target[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i = 0; i < 6; ++i) {
        if (each_face[i].empty()) throw std::runtime_error("load file failed");

        if (each_face[i].type() == CV_8UC3)
            glTexImage2D(face_target[i], 0, GL_RGB8, each_face[i].cols, each_face[i].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, each_face[i].data);
        else if (each_face[i].type() == CV_8UC4)
            glTexImage2D(face_target[i], 0, GL_RGBA8, each_face[i].cols, each_face[i].rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, each_face[i].data);
        each_face[i].release();
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 恢復原狀
    glBindTexture(GL_TEXTURE_CUBE_MAP, old_cube_map);
}

qtTextureCubeMap::~qtTextureCubeMap()
{
    glDeleteTextures(1, &m_texture_id);
}

void qtTextureCubeMap::bind_to(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);
}

void qtTextureCubeMap::unbind_from(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
