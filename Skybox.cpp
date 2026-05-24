#include "Skybox.h"
#include "Shader.h"
#include "TextureLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Unit cube that fills clip space; we strip translation from the view matrix
static const float CUBE_VERTS[] = {
    -1, -1, -1,   1, -1, -1,   1,  1, -1,  -1,  1, -1,  // -Z
    -1, -1,  1,   1, -1,  1,   1,  1,  1,  -1,  1,  1,  // +Z
    -1,  1,  1,  -1,  1, -1,  -1, -1, -1,  -1, -1,  1,  // -X
     1,  1,  1,   1,  1, -1,   1, -1, -1,   1, -1,  1,  // +X
    -1, -1, -1,   1, -1, -1,   1, -1,  1,  -1, -1,  1,  // -Y
    -1,  1, -1,   1,  1, -1,   1,  1,  1,  -1,  1,  1,  // +Y
};

static const unsigned int CUBE_IDX[] = {
     0, 1, 2,  0, 2, 3,
     4, 6, 5,  4, 7, 6,
     8, 9,10,  8,10,11,
    12,14,13, 12,15,14,
    16,17,18, 16,18,19,
    20,22,21, 20,23,22,
};

static void buildFacePaths(const std::string &prefix, std::string out[6]) {
    // OpenGL cubemap face order: +X (Right), -X (Left), +Y (Top), -Y (Bottom), +Z (Back), -Z (Front)
    // We swapped "back" and "front" in this array so they map to +Z and -Z correctly
    const char *faces[6] = {"right", "left", "top", "bottom", "back", "front"};
    for (int i = 0; i < 6; ++i)
        out[i] = prefix + faces[i] + ".png";
}

Skybox::Skybox() : isNight(false) {
    // GPU buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindVertexArray(0);

    // Index buffer is tiny — embed in a second buffer attached to the same VAO
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBE_IDX), CUBE_IDX, GL_STATIC_DRAW);
    glBindVertexArray(0);

    std::string dayPaths[6], nightPaths[6];
    buildFacePaths("textures/skybox_day_",   dayPaths);
    buildFacePaths("textures/skybox_night_", nightPaths);

    cubemapDay   = TextureLoader::loadCubemap(dayPaths);
    cubemapNight = TextureLoader::loadCubemap(nightPaths);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Skybox::setNight(bool night) {
    isNight = night;
}

void Skybox::draw(Shader &shader, glm::mat4 view, glm::mat4 projection) {
    // Strip translation so the skybox stays centred on the camera
    glm::mat4 skyView = glm::mat4(glm::mat3(view));

    glDepthFunc(GL_LEQUAL);
    
    // FIX: Disable Face Culling specifically for the skybox so all inside faces render!
    glDisable(GL_CULL_FACE); 

    shader.use();
    shader.setMat4("view",       skyView);
    shader.setMat4("projection", projection);
    shader.setInt("skybox", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, isNight ? cubemapNight : cubemapDay);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // Restore Face Culling and depth testing for the rest of the golf course
    glEnable(GL_CULL_FACE); 
    glDepthFunc(GL_LESS);
}