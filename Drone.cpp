#include "Drone.h"
#include "Shader.h"
#include "MathHelpers.h"

#include <GL/glew.h>
#include <cmath>

static const float PI = 3.14159265f;



static Mesh makeCone(float radius, float height, int segments) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    
    verts.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}});

    
    for (int i = 0; i <= segments; ++i) {
        float t  = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float cx = cosf(t);
        float cz = sinf(t);
        
        glm::vec3 n = glm::normalize(glm::vec3(cx * height, radius, cz * height));
        verts.push_back({{cx * radius, -height, cz * radius}, n,
                          {cx * 0.5f + 0.5f, 0.0f}});
    }

    for (int i = 0; i < segments; ++i) {
        idx.push_back(0);
        idx.push_back(static_cast<unsigned int>(i + 1));
        idx.push_back(static_cast<unsigned int>(i + 2));
    }

    return Mesh(verts, idx);
}

Drone::Drone()
    : body  (Mesh::createBox(0.28f, 0.065f, 0.28f))
    , camBox(Mesh::createBox(0.07f, 0.050f, 0.07f))
    , arm   (Mesh::createBox(0.42f, 0.020f, 0.034f))
    , rotor (Mesh::createCylinder(0.11f, 0.012f, 16))
    , cone  (makeCone(1.0f, 1.0f, 20))
{}

void Drone::draw(Shader &shader,
                 glm::vec3 camPos,
                 glm::vec3 camFront,
                 float     rotorAngle)
{
    
    glm::vec3 hFront = glm::vec3(camFront.x, 0.0f, camFront.z);
    float hLen = glm::length(hFront);
    if (hLen < 0.001f) hFront = glm::vec3(0.0f, 0.0f, -1.0f);
    else                hFront /= hLen;
    glm::vec3 hRight = glm::normalize(glm::cross(hFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    
    glm::vec3 dronePos = camPos + glm::vec3(0.0f, -0.85f, 0.0f);

    
    glm::mat4 base(1.0f);
    base[0] = glm::vec4(hRight,                   0.0f);
    base[1] = glm::vec4(0.0f, 1.0f, 0.0f,         0.0f);
    base[2] = glm::vec4(hFront,                   0.0f);
    base[3] = glm::vec4(dronePos,                 1.0f);

    shader.use();
    shader.setFloat("objectAlpha", 1.0f);
    shader.setInt  ("useTexture",  0);

    
    shader.setVec3("objectColor", glm::vec3(0.12f, 0.12f, 0.12f));
    shader.setMat4("model", base);
    body.draw();

    
    glm::mat4 camBoxM = base * makeTranslate(glm::vec3(0.0f, -0.057f, 0.0f));
    shader.setVec3("objectColor", glm::vec3(0.08f, 0.08f, 0.08f));
    shader.setMat4("model", camBoxM);
    camBox.draw();

    
    
    
    const float ARM_ANGLES[2] = {-45.0f, 45.0f};
    shader.setVec3("objectColor", glm::vec3(0.18f, 0.18f, 0.18f));
    for (int i = 0; i < 2; ++i) {
        glm::mat4 armM = base * makeRotate(degToRad(ARM_ANGLES[i]),
                           glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", armM);
        arm.draw();
    }

    
    
    const float ARM_HALF = 0.21f;  
    const float A = ARM_HALF * 0.7071f;  
    const float tipX[4] = { A, -A, -A,  A};
    const float tipZ[4] = { A,  A, -A, -A};

    shader.setVec3("objectColor", glm::vec3(0.25f, 0.25f, 0.25f));
    for (int i = 0; i < 4; ++i) {
        glm::vec3 tipLocal(tipX[i], 0.0f, tipZ[i]);
        glm::mat4 rotorM = base * makeTranslate(tipLocal);
        
        float spin = (i % 2 == 0) ? rotorAngle : -rotorAngle;
        rotorM = rotorM * makeRotate(degToRad(spin),
                         glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", rotorM);
        rotor.draw();
    }
}

void Drone::drawSpotlightCone(Shader &shader, glm::vec3 camPos)
{
    
    
    const float DEPTH       = 9.0f;
    const float CONE_RADIUS = DEPTH * std::tan(degToRad(15.0f));

    glm::vec3 apex = camPos + glm::vec3(0.0f, -0.90f, 0.0f);
    glm::mat4 coneM = makeTranslate(apex) * makeScale(glm::vec3(CONE_RADIUS, DEPTH, CONE_RADIUS));

    shader.use();
    shader.setVec3 ("objectColor", glm::vec3(0.95f, 0.95f, 0.70f));
    shader.setInt  ("useTexture",  0);
    shader.setFloat("objectAlpha", 0.13f);
    shader.setMat4 ("model", coneM);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    cone.draw();

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    shader.setFloat("objectAlpha", 1.0f);
}
