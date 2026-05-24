#include "Objects.h"
#include "Shader.h"
#include "TextureLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

// Helper: set common shader uniforms and bind a texture to unit 0
static void bindTex(Shader &shader, GLuint tex) {
    shader.setInt("diffuseTex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

// ---- Rock ------------------------------------------------------------------

Rock::Rock(glm::vec3 scale)
    : mesh(Mesh::createSphere(0.5f, 16))
    , texture(TextureLoader::load("textures/rock.png"))
    , localScale(scale)
{}

void Rock::draw(Shader &shader, glm::mat4 modelMatrix) {
    glm::mat4 m = glm::scale(modelMatrix, localScale);
    shader.use();
    shader.setMat4("model", m);
    bindTex(shader, texture);
    mesh.draw();
}

// ---- Barrel ----------------------------------------------------------------

// Flat disk cap: just a cylinder of near-zero height subdivided enough to look solid
static Mesh makeDisk(float radius) {
    return Mesh::createCylinder(radius, 0.02f, 24);
}

Barrel::Barrel(bool sw)
    : body(Mesh::createCylinder(0.35f, 0.7f, 24))
    , capTop(makeDisk(0.35f))
    , capBot(makeDisk(0.35f))
    , texture(TextureLoader::load("textures/wood.png"))
    , sideways(sw)
{}

void Barrel::draw(Shader &shader, glm::mat4 modelMatrix) {
    glm::mat4 base = modelMatrix;
    if (sideways)
        base = glm::rotate(base, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    shader.use();
    bindTex(shader, texture);

    // Body
    shader.setMat4("model", base);
    body.draw();

    // Top cap
    glm::mat4 topM = glm::translate(base, glm::vec3(0.0f, 0.35f, 0.0f));
    shader.setMat4("model", topM);
    capTop.draw();

    // Bottom cap
    glm::mat4 botM = glm::translate(base, glm::vec3(0.0f, -0.35f, 0.0f));
    shader.setMat4("model", botM);
    capBot.draw();
}

// ---- LogBarrier ------------------------------------------------------------

LogBarrier::LogBarrier()
    : mesh(Mesh::createCylinder(0.2f, 3.0f, 16))
    , texture(TextureLoader::load("textures/bark.png"))
{}

void LogBarrier::draw(Shader &shader, glm::mat4 modelMatrix) {
    glm::mat4 m = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.use();
    shader.setMat4("model", m);
    bindTex(shader, texture);
    mesh.draw();
}

// ---- Billboard -------------------------------------------------------------

// A single unit quad in XY plane centred at origin
static Mesh makeQuad() {
    std::vector<Vertex> v = {
        {{-0.5f, -0.5f, 0.0f}, {0,0,1}, {0,0}},
        {{ 0.5f, -0.5f, 0.0f}, {0,0,1}, {1,0}},
        {{ 0.5f,  0.5f, 0.0f}, {0,0,1}, {1,1}},
        {{-0.5f,  0.5f, 0.0f}, {0,0,1}, {0,1}},
    };
    std::vector<unsigned int> idx = {0,1,2, 0,2,3};
    return Mesh(v, idx);
}

Billboard::Billboard(const std::string &texturePath, const glm::vec3 &cameraPos)
    : quad(makeQuad())
    , texture(TextureLoader::load(texturePath))
    , camPosition(cameraPos)
{}

void Billboard::updateCamera(const glm::vec3 &camPos) {
    camPosition = camPos;
}

void Billboard::draw(Shader &shader, glm::mat4 modelMatrix) {
    // Extract translation from model matrix; build a billboard rotation toward camera
    glm::vec3 worldPos = glm::vec3(modelMatrix[3]);
    glm::vec3 toCamera = glm::normalize(camPosition - worldPos);
    toCamera.y = 0.0f;
    if (glm::length(toCamera) < 0.001f) toCamera = glm::vec3(0, 0, 1);
    toCamera = glm::normalize(toCamera);

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(up, toCamera));

    glm::mat4 rot(1.0f);
    rot[0] = glm::vec4(right,   0.0f);
    rot[1] = glm::vec4(up,      0.0f);
    rot[2] = glm::vec4(toCamera,0.0f);

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(modelMatrix[0][0],
                                                              modelMatrix[1][1],
                                                              modelMatrix[2][2]));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), worldPos);
    glm::mat4 m     = trans * rot * scale;

    shader.use();
    shader.setMat4("model", m);
    bindTex(shader, texture);
    quad.draw();

    // Second quad rotated 90° to form a cross
    glm::mat4 m2 = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", m2);
    quad.draw();
}

// ---- FlagPole --------------------------------------------------------------

FlagPole::FlagPole(glm::vec3 flagColor)
    : pole(Mesh::createCylinder(0.05f, 4.0f, 8))
    , flag(makeQuad())
    , color(flagColor)
{}

void FlagPole::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();
    bindTex(shader, TextureLoader::load("textures/metal.png"));

    shader.setMat4("model", modelMatrix);
    pole.draw();

    // Flag sits at the top, offset by half pole height
    glm::mat4 flagM = glm::translate(modelMatrix, glm::vec3(0.3f, 2.2f, 0.0f));
    flagM = glm::scale(flagM, glm::vec3(0.6f, 0.4f, 1.0f));
    shader.setMat4("model", flagM);
    // Tint via objectColor
    shader.setVec3("objectColor", color);
    TextureLoader::load("textures/flag.png");
    bindTex(shader, TextureLoader::load("textures/flag.png"));
    flag.draw();
    shader.setVec3("objectColor", glm::vec3(1.0f));  // reset
}

// ---- Bridge ----------------------------------------------------------------

Bridge::Bridge(float width, float length, float thickness)
    : mesh(Mesh::createBox(width, thickness, length))
    , texture(TextureLoader::load("textures/wood.png"))
{}

void Bridge::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();
    shader.setMat4("model", modelMatrix);
    bindTex(shader, texture);
    mesh.draw();
}

// ---- LightPole -------------------------------------------------------------

LightPole::LightPole()
    : pole(Mesh::createCylinder(0.06f, 6.0f, 8))
    , cap(Mesh::createBox(0.4f, 0.15f, 0.6f))
{}

void LightPole::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();

    shader.setMat4("model", modelMatrix);
    bindTex(shader, TextureLoader::load("textures/metal.png"));
    pole.draw();

    // Cap sits at top, slightly angled
    glm::mat4 capM = glm::translate(modelMatrix, glm::vec3(0.0f, 3.1f, 0.1f));
    capM = glm::rotate(capM, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", capM);
    cap.draw();
}

// ---- Windmill (placeholder) ------------------------------------------------

Windmill::Windmill()
    : body(Mesh::createBox(2.0f, 4.0f, 2.0f))
{}

void Windmill::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();
    shader.setMat4("model", modelMatrix);
    bindTex(shader, TextureLoader::load("textures/wood.png"));
    body.draw();
}
