#include "TextureLoader.h"

// stb_image implementation compiled exactly once, here
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#include <iostream>
#include <unordered_map>

static std::unordered_map<std::string, GLuint> g_cache;

GLuint TextureLoader::load(const std::string &path) {
    auto it = g_cache.find(path);
    if (it != g_cache.end())
        return it->second;

    stbi_set_flip_vertically_on_load(true);

    int w, h, channels;
    unsigned char *data = stbi_load(path.c_str(), &w, &h, &channels, 0);

    GLuint id = 0;
    if (!data) {
        std::cerr << "[TextureLoader] Failed to load: " << path << "\n";
        // Return a 1×1 white fallback so the object still renders
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        unsigned char white[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        GLenum fmt = (channels == 4) ? GL_RGBA : (channels == 3) ? GL_RGB : GL_RED;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(fmt), w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }

    g_cache[path] = id;
    return id;
}

GLuint TextureLoader::loadCubemap(const std::string paths[6]) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    stbi_set_flip_vertically_on_load(false);  // cubemap faces must NOT be flipped

    for (int i = 0; i < 6; ++i) {
        int w, h, channels;
        unsigned char *data = stbi_load(paths[i].c_str(), &w, &h, &channels, 0);
        if (data) {
            GLenum fmt = (channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, static_cast<GLint>(fmt), w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "[TextureLoader] Cubemap face missing: " << paths[i] << "\n";
            // 1×1 placeholder face
            unsigned char col[3] = {100, 149, 237};  // cornflower blue
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, col);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}
