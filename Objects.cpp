#include "Objects.h"
#include "Shader.h"
#include "TextureLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static void bindTex(Shader &shader, GLuint tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    shader.setInt("objectTexture", 0);
    shader.setInt("useTexture", 1);
}



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
    shader.setInt("useTexture", 0);
}




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

    
    shader.setMat4("model", base);
    body.draw();

    
    glm::mat4 topM = glm::translate(base, glm::vec3(0.0f, 0.35f, 0.0f));
    shader.setMat4("model", topM);
    capTop.draw();

    
    glm::mat4 botM = glm::translate(base, glm::vec3(0.0f, -0.35f, 0.0f));
    shader.setMat4("model", botM);
    capBot.draw();
    shader.setInt("useTexture", 0);
}



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
    shader.setInt("useTexture", 0);
}




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
    
    glm::vec3 worldPos = glm::vec3(modelMatrix[3]);
    glm::vec3 toCamera = glm::normalize(camPosition - worldPos);
    toCamera.y = 0.0f;
    if (glm::length(toCamera) < 0.001f) toCamera = glm::vec3(0, 0, 1);
    toCamera = glm::normalize(toCamera);

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(up, toCamera));

    glm::mat4 rot(1.0f);
    rot[0] = glm::vec4(right,0.0f);
    rot[1] = glm::vec4(up,0.0f);
    rot[2] = glm::vec4(toCamera,0.0f);

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(modelMatrix[0][0],modelMatrix[1][1],modelMatrix[2][2]));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), worldPos);
    glm::mat4 m = trans * rot * scale;

    shader.use();
    shader.setMat4("model", m);
    bindTex(shader, texture);
    quad.draw();

    
    glm::mat4 m2 = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", m2);
    quad.draw();
    shader.setInt("useTexture", 0);
}



FlagPole::FlagPole(glm::vec3 flagColor)
    : pole(Mesh::createCylinder(0.03f, 1.2f, 8))
    , flag(makeQuad())
    , color(flagColor)
{}

void FlagPole::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();

    
    bindTex(shader, TextureLoader::load("textures/metal.png"));
    shader.setMat4("model", modelMatrix);
    pole.draw();

    
    bindTex(shader, TextureLoader::load("textures/flag.png"));
    glm::mat4 flagM = glm::translate(modelMatrix, glm::vec3(0.15f, 0.5f, 0.0f));
    flagM = glm::scale(flagM, glm::vec3(0.5f, 0.3f, 1.0f));
    shader.setMat4("model", flagM);
    flag.draw();

    shader.setInt("useTexture", 0);
    shader.setVec3("objectColor", glm::vec3(1.0f));
}



Bridge::Bridge(float width, float length, float thickness)
    : mesh(Mesh::createBox(width, thickness, length))
    , texture(TextureLoader::load("textures/wood.png"))
{}

void Bridge::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();
    shader.setMat4("model", modelMatrix);
    bindTex(shader, texture);
    mesh.draw();
    shader.setInt("useTexture", 0);
}



LightPole::LightPole()
    : pole(Mesh::createCylinder(0.06f, 6.0f, 8))
    , cap(Mesh::createBox(0.4f, 0.15f, 0.6f))
{}

void LightPole::draw(Shader &shader, glm::mat4 modelMatrix) {
    shader.use();

    shader.setMat4("model", modelMatrix);
    bindTex(shader, TextureLoader::load("textures/metal.png"));
    pole.draw();

    
    glm::mat4 capM = glm::translate(modelMatrix, glm::vec3(0.0f, 3.1f, 0.1f));
    capM = glm::rotate(capM, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", capM);
    cap.draw();
    shader.setInt("useTexture", 0);
}



Windmill::Windmill()
    : bodyTop(Mesh::createBox(0.70f, 1.00f, 0.70f))
    , bodyMid(Mesh::createBox(0.85f, 1.00f, 0.85f))
    , bodyBot(Mesh::createBox(1.00f, 1.00f, 1.00f))
    , baseLeft(Mesh::createBox(0.36f, 0.80f, 1.20f))
    , baseRight(Mesh::createBox(0.36f, 0.80f, 1.20f))
    , baseLintel(Mesh::createBox(1.20f, 0.20f, 1.20f))
    , roof(Mesh::createTriangularPrism(0.70f, 0.55f, 0.70f))
    , axle(Mesh::createCylinder(0.05f, 0.35f, 16))
    , hub(Mesh::createBox(0.15f, 0.15f, 0.12f))
    , blade(Mesh::createBox(3.00f, 0.10f, 0.06f))
{}

void Windmill::draw(Shader &shader, glm::mat4 modelMatrix, float spinAngle) {
    shader.use();
    bindTex(shader, TextureLoader::load("textures/wood.png"));

    
    glm::mat4 bodyBotM = glm::translate(modelMatrix, glm::vec3(0.0f, -0.50f, 0.0f));
    shader.setMat4("model", bodyBotM);
    bodyBot.draw();

    glm::mat4 bodyMidM = glm::translate(modelMatrix, glm::vec3(0.0f, 0.50f, 0.0f));
    shader.setMat4("model", bodyMidM);
    bodyMid.draw();

    glm::mat4 bodyTopM = glm::translate(modelMatrix, glm::vec3(0.0f, 1.50f, 0.0f));
    shader.setMat4("model", bodyTopM);
    bodyTop.draw();

    
    glm::mat4 leftPillar = glm::translate(modelMatrix, glm::vec3(-0.27f, -2.00f, 0.0f));
    shader.setMat4("model", leftPillar);
    baseLeft.draw();

    glm::mat4 rightPillar = glm::translate(modelMatrix, glm::vec3(0.27f, -2.00f, 0.0f));
    shader.setMat4("model", rightPillar);
    baseRight.draw();

    glm::mat4 lintel = glm::translate(modelMatrix, glm::vec3(0.0f, -1.60f, 0.0f));
    shader.setMat4("model", lintel);
    baseLintel.draw();

    
    glm::mat4 roofM = glm::translate(modelMatrix, glm::vec3(0.0f, 2.00f, 0.0f));
    shader.setMat4("model", roofM);
    roof.draw();

    
    glm::mat4 axleM = glm::translate(modelMatrix, glm::vec3(0.0f, 1.50f, 0.42f));
    shader.setMat4("model", axleM);
    bindTex(shader, TextureLoader::load("textures/metal.png"));
    axle.draw();

    glm::mat4 hubM = glm::translate(modelMatrix, glm::vec3(0.0f, 1.50f, 0.42f));
    shader.setMat4("model", hubM);
    bindTex(shader, TextureLoader::load("textures/metal.png"));
    hub.draw();

    
    const float PI = 3.14159265f;
    const float bladeAngles[4] = {PI * 0.25f, PI * 0.75f, PI * 1.25f, PI * 1.75f};
    for (int i = 0; i < 4; ++i) {
        glm::mat4 bladeM = glm::translate(modelMatrix, glm::vec3(0.0f, 1.50f, 0.42f));
        bladeM = glm::rotate(bladeM, glm::radians(spinAngle) + bladeAngles[i], glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", bladeM);
        bindTex(shader, TextureLoader::load("textures/wood.png"));
        blade.draw();
    }
    shader.setInt("useTexture", 0);
}
