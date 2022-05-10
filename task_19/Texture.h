#pragma once
#include <GL/freeglut.h>
#include <string>
#include <Magick++/Image.h>
#include <Magick++/Blob.h>
#include <iostream>

class Texture
{
    std::string m_fileName;
    Magick::Image* m_pImage = nullptr;
    Magick::Blob m_blob;
    GLenum m_textureTarget;
    GLuint m_textureObj;

public:
    Texture(GLenum TextureTarget, const std::string& FileName) : m_textureTarget(TextureTarget), m_fileName(FileName) {}

    bool Load() {
        try {
            m_pImage = new Magick::Image(m_fileName);
            m_pImage->write(&m_blob, "RGBA");
        }
        catch (Magick::Error& Error) {
            std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
            return false;
        }

        glGenTextures(1, &m_textureObj);
        glBindTexture(m_textureTarget, m_textureObj);
        glTexImage2D(m_textureTarget, 0, GL_RGB, m_pImage->columns(), m_pImage->rows(), -0.5, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
        glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return true;
    };

    void Bind(GLenum TextureUnit)
    {
        glActiveTexture(TextureUnit);
        glBindTexture(m_textureTarget, m_textureObj);
    }
};