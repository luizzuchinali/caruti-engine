#include "Texture.hpp"

namespace Graphics {

    Texture::Texture(const char *texPath, GLenum channelFormat, GLenum index, GLint wrap) : _index(index) {
        const std::string basePath = "resources/textures/";

        stbi_set_flip_vertically_on_load(true);

        glGenTextures(1, &_id);
        glBindTexture(GL_TEXTURE_2D, _id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        unsigned char *buffer = stbi_load(
                (basePath + texPath).c_str(),
                &_width, &_height,
                &_nrChannels, 0);

        if (buffer) {
            glTexImage2D(GL_TEXTURE_2D, 0, channelFormat, _width, _height, 0, channelFormat, GL_UNSIGNED_BYTE, buffer);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            Log::Error("TEXTURE::LOAD_FAILED {}", texPath);
        }
        stbi_image_free(buffer);
    }

    void Texture::ActivateAndBind() const {
        glActiveTexture(_index);
        glBindTexture(GL_TEXTURE_2D, _id);
    }

    void Texture::ActivateAndBind(GLenum texIndex) {
        _index = texIndex;
        glActiveTexture(_index);
        glBindTexture(GL_TEXTURE_2D, _id);
    }

    unsigned int Texture::GetId() const {
        return _id;
    }

    int Texture::GetWidth() const {
        return _width;
    }

    int Texture::GetHeight() const {
        return _height;
    }

    int Texture::GetNrChannels() const {
        return _nrChannels;
    }

    int Texture::GetIndex() const {

        switch (_index) {
            case GL_TEXTURE0:
                return 0;
            case GL_TEXTURE1:
                return 1;
            case GL_TEXTURE2:
                return 2;
            case GL_TEXTURE3:
                return 3;
            case GL_TEXTURE4:
                return 4;
            case GL_TEXTURE5:
                return 5;
            case GL_TEXTURE6:
                return 6;
        }

        return -1;
    }

}