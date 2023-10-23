#pragma once

#include <iostream>
#include "glad/glad.h"
#include "stb_image.h"
#include "Log.hpp"

namespace Graphics {

    class Texture {
    public:
        Texture(const char *texPath, GLenum channelFormat, GLenum index);

        void ActivateAndBind() const;

        void ActivateAndBind(GLenum texIndex);

        unsigned int GetId() const;

        int GetWidth() const;

        int GetHeight() const;

        int GetNrChannels() const;

        int GetIndex() const;

    private:
        unsigned int _id{};
        int _width{};
        int _height{};
        int _nrChannels{};

        GLenum _index{};
    };

}