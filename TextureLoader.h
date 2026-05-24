#pragma once

#include <GL/glew.h>
#include <string>

class TextureLoader {
public:
    // Returns a GL texture ID. Repeated calls with the same path return the cached ID.
    static GLuint load(const std::string &path);

    // Load a cubemap from 6 face images (right,left,top,bottom,front,back).
    static GLuint loadCubemap(const std::string paths[6]);
};
