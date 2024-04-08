#pragma once

#include <Headers.hpp>

class Texture
{
    public:
        GLuint texID, texWidth, texHeight, texComponents;
        GLfloat anisoFilterLevel;
        GLenum texType, texInternalFormat, texFormat;
        std::string texName;
        unsigned int id;

        Texture();
        ~Texture();
        void createTexture(int TEXTURE_WIDTH, int TEXTURE_HEIGHT);
        void setTexture(const char* texPath, std::string texName, bool texFlip);
        void useTexture();

        GLuint getTexID();
        GLuint getTexWidth();
        GLuint getTexHeight();
        std::string getTexName();
};
