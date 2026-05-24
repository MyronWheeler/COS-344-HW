#include "Drone.h"
#include "Shader.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

static const float PI = 3.14159265f;

// Cone mesh: apex at origin, opens downward to radius r at y = -h.
// Used for the transparent spotlight beam visual.
static Mesh makeCone(float radius, float height, int segments) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    // Apex
    verts.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f}});

    // Base ring at y = -height
    for (int i = 0; i <= segments; ++i) {
        float t  = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
        float cx = cosf(t);
        float cz = sinf(t);
        // Outward slant normal (points away from cone axis and slightly upward)
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
    // Project the camera front onto the horizontal plane so the drone stays level.
    glm::vec3 hFront = glm::vec3(camFront.x, 0.0f, camFront.z);
    float hLen = glm::length(hFront);
    if (hLen < 0.001f) hFront = glm::vec3(0.0f, 0.0f, -1.0f);
    else                hFront /= hLen;
    glm::vec3 hRight = glm::normalize(glm::cross(hFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Drone sits 0.85 units below the camera origin.
    glm::vec3 dronePos = camPos + glm::vec3(0.0f, -0.85f, 0.0f);

    // Build a local-to-world basis: X = right, Y = up, Z = horizontal front.
    glm::mat4 base(1.0f);
    base[0] = glm::vec4(hRight,                   0.0f);
    base[1] = glm::vec4(0.0f, 1.0f, 0.0f,         0.0f);
    base[2] = glm::vec4(hFront,                   0.0f);
    base[3] = glm::vec4(dronePos,                 1.0f);

    shader.use();
    shader.setFloat("objectAlpha", 1.0f);
    shader.setInt  ("useTexture",  0);

    // ---- Body ---------------------------------------------------------------
    shader.setVec3("objectColor", glm::vec3(0.12f, 0.12f, 0.12f));
    shader.setMat4("model", base);
    body.draw();

    // ---- Camera / spotlight housing (below body) ----------------------------
    glm::mat4 camBoxM = glm::translate(base, glm::vec3(0.0f, -0.057f, 0.0f));
    shader.setVec3("objectColor", glm::vec3(0.08f, 0.08f, 0.08f));
    shader.setMat4("model", camBoxM);
    camBox.draw();

    // ---- Arms (two bars crossing at centre, ±45° from drone front) ----------
    // Arm A: rotated -45° — connects front-right tip to back-left tip.
    // Arm B: rotated +45° — connects front-left tip to back-right tip.
    const float ARM_ANGLES[2] = {-45.0f, 45.0f};
    shader.setVec3("objectColor", glm::vec3(0.18f, 0.18f, 0.18f));
    for (int i = 0; i < 2; ++i) {
        glm::mat4 armM = glm::rotate(base, glm::radians(ARM_ANGLES[i]),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", armM);
        arm.draw();
    }

    // ---- Rotors (four flat disks, one at each arm tip) ----------------------
    // Tip directions in local drone space (±45° diagonals, ARM_HALF from centre).
    const float ARM_HALF = 0.21f;  // half of 0.42 arm length
    const float A = ARM_HALF * 0.7071f;  // = ARM_HALF * cos/sin(45°)
    const float tipX[4] = { A, -A, -A,  A};
    const float tipZ[4] = { A,  A, -A, -A};

    shader.setVec3("objectColor", glm::vec3(0.25f, 0.25f, 0.25f));
    for (int i = 0; i < 4; ++i) {
        glm::vec3 tipLocal(tipX[i], 0.0f, tipZ[i]);
        glm::mat4 rotorM = glm::translate(base, tipLocal);
        // Alternate rotors spin opposite directions for visual variety.
        float spin = (i % 2 == 0) ? rotorAngle : -rotorAngle;
        rotorM = glm::rotate(rotorM, glm::radians(spin),
                             glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", rotorM);
        rotor.draw();
    }
}

void Drone::drawSpotlightCone(Shader &shader, glm::vec3 camPos)
{
    // Cone apex just below the camera housing; opens straight down.
    // Inner spotlight cutoff is 15°: at depth d the radius = d * tan(15°).
    const float DEPTH       = 9.0f;
    const float CONE_RADIUS = DEPTH * tanf(glm::radians(15.0f));

    glm::vec3 apex = camPos + glm::vec3(0.0f, -0.90f, 0.0f);
    glm::mat4 coneM = glm::translate(glm::mat4(1.0f), apex);
    coneM = glm::scale(coneM, glm::vec3(CONE_RADIUS, DEPTH, CONE_RADIUS));

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
