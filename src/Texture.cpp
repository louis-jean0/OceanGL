#include <Texture.hpp>

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF


Texture::Texture()
{

}

Texture::~Texture()
{
    glDeleteTextures(1, &this->texID);
}

void Texture::setTexture(const char* texPath, std::string texName, bool texFlip)
{
    this->texType = GL_TEXTURE_2D;

    std::string tempPath = std::string(texPath);

    if(texFlip)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &this->texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texID);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoFilterLevel); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, this->anisoFilterLevel);

    int width, height, numComponents;
    unsigned char* texData = stbi_load(tempPath.c_str(), &width, &height, &numComponents, 0);

    this->texWidth = width;
    this->texHeight = height;
    this->texComponents = numComponents;
    this->texName = texName;

    if (texData)
    {
        if (numComponents == 1)
            this->texFormat = GL_RED;
        else if (numComponents == 3)
            this->texFormat = GL_RGB;
        else if (numComponents == 4)
            this->texFormat = GL_RGBA;
        this->texInternalFormat = this->texFormat;

        glTexImage2D(GL_TEXTURE_2D, 0, this->texInternalFormat, this->texWidth, this->texHeight, 0, this->texFormat, GL_UNSIGNED_BYTE, texData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    else
    {
        std::cerr << "TEXTURE FAILED - LOADING : " << texPath << std::endl;
    }

    stbi_image_free(texData);

    glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::createTexture(int TEXTURE_WIDTH, int TEXTURE_HEIGHT) {
	glGenTextures(1, &this->texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, this->texID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texID);   
}

void Texture::useTexture()
{
    glBindTexture(this->texType, this->texID);
}

GLuint Texture::getTexID()
{
    return this->texID;
}

GLuint Texture::getTexWidth()
{
    return this->texWidth;
}

GLuint Texture::getTexHeight()
{
    return this->texHeight;
}

std::string Texture::getTexName()
{
    return this->texName;
}


