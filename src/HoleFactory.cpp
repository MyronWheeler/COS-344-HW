#include "HoleFactory.h"
#include "Shader.h"
#include "TextureLoader.h"
#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// ---------------------------------------------------------------------------
// Stream helper — builds one rectangular plane segment between two waypoints
// ---------------------------------------------------------------------------
static std::pair<Mesh, glm::mat4>
makeStreamSegment(glm::vec2 a, glm::vec2 b, float width) {
    glm::vec2 dir  = b - a;
    float     len  = glm::length(dir);
    if (len < 0.001f) len = 0.001f;

    // The mesh is a unit quad; we scale and orient it via the transform
    std::vector<Vertex> verts = {
        {{-0.5f, 0.0f, -0.5f}, {0,1,0}, {0,0}},
        {{ 0.5f, 0.0f, -0.5f}, {0,1,0}, {1,0}},
        {{ 0.5f, 0.0f,  0.5f}, {0,1,0}, {1,1}},
        {{-0.5f, 0.0f,  0.5f}, {0,1,0}, {0,1}},
    };
    std::vector<unsigned int> idx = {0,1,2, 0,2,3};
    Mesh seg(verts, idx);

    glm::vec3 mid((a.x + b.x) * 0.5f, 0.01f, (a.y + b.y) * 0.5f);
    float angle = std::atan2(dir.x, dir.y);  // angle in XZ

    glm::mat4 t = glm::translate(glm::mat4(1.0f), mid);
    t = glm::rotate(t, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    t = glm::scale(t, glm::vec3(width, 1.0f, len));

    return {std::move(seg), t};
}

// ---------------------------------------------------------------------------
// HoleFactory::build
// ---------------------------------------------------------------------------
HoleNode HoleFactory::build(const HoleConfig &cfg) {
    HoleNode node;

    // World transform for the whole hole
    node.worldTransform = glm::translate(glm::mat4(1.0f), cfg.position);
    node.worldTransform = glm::rotate(node.worldTransform,
                                      glm::radians(cfg.rotation),
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    // Terrain — needs at least 3 boundary points
    if (cfg.boundaryPoints.size() >= 3) {
        std::vector<float> elevs = cfg.elevations;
        // Pad elevations with zeros if under-specified
        while (elevs.size() < cfg.boundaryPoints.size())
            elevs.push_back(0.0f);
        node.terrain = std::unique_ptr<Terrain>(
            new Terrain(cfg.boundaryPoints, elevs));
    }

    // Stream segments
    if (cfg.hasStream && cfg.streamPath.size() >= 2) {
        for (size_t i = 0; i + 1 < cfg.streamPath.size(); ++i) {
            node.streamSegments.push_back(
                makeStreamSegment(cfg.streamPath[i], cfg.streamPath[i + 1], 1.5f));
        }
    }

    // Pond — flattened sphere at Y ≈ 0
    if (cfg.hasPond) {
        node.hasPond = true;
        node.pond    = std::unique_ptr<Mesh>(
            new Mesh(Mesh::createSphere(cfg.pondRadius, 32)));
        node.pondTransform = glm::scale(glm::mat4(1.0f),
                                        glm::vec3(1.0f, 0.05f, 1.0f));
        node.pondTransform = glm::translate(glm::mat4(1.0f),
                                            glm::vec3(0.0f, 0.02f, 0.0f))
                             * node.pondTransform;
    }

    // Bridge — flat box spanning the stream at the first bridge-crossing waypoint
    if (cfg.hasBridge && cfg.streamPath.size() >= 2) {
        node.hasBridge = true;
        node.bridge    = std::unique_ptr<Mesh>(
            new Mesh(Mesh::createBox(2.0f, 0.1f, 1.6f)));

        glm::vec2 mid = (cfg.streamPath[0] + cfg.streamPath[1]) * 0.5f;
        glm::vec2 dir = cfg.streamPath[1] - cfg.streamPath[0];
        float angle   = std::atan2(dir.x, dir.y);

        node.bridgeTransform = glm::translate(glm::mat4(1.0f),
                                              glm::vec3(mid.x, 0.06f, mid.y));
        node.bridgeTransform = glm::rotate(node.bridgeTransform, angle,
                                           glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Obstacles
    for (const auto &e : cfg.obstacles) {
        HoleNode::PlacedObject po;
        po.type = e.type;
        po.transform = glm::translate(glm::mat4(1.0f), e.localPos);
        po.transform = glm::rotate(po.transform, glm::radians(e.rotation),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
        po.transform = glm::scale(po.transform, e.scale);
        node.obstacles.push_back(std::move(po));
    }

    // Decor
    for (const auto &e : cfg.decor) {
        HoleNode::PlacedObject po;
        po.type = e.type;
        po.transform = glm::translate(glm::mat4(1.0f), e.localPos);
        po.transform = glm::rotate(po.transform, glm::radians(e.rotation),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
        po.transform = glm::scale(po.transform, e.scale);
        node.decor.push_back(std::move(po));
    }

    return node;
}

// ---------------------------------------------------------------------------
// HoleNode::draw
// ---------------------------------------------------------------------------
void HoleNode::draw(Shader &shader) {
    // Terrain
    if (terrain)
        terrain->draw(shader, worldTransform);

    // Stream segments
    {
        GLuint waterTex = TextureLoader::load("textures/water_normal.png");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTex);
        shader.setInt("objectTexture", 0);
        shader.setVec3("objectColor", glm::vec3(0.1f, 0.35f, 0.5f));
        for (auto &seg : streamSegments) {
            shader.setMat4("model", worldTransform * seg.second);
            seg.first.draw();
        }
    }

    // Pond
    if (hasPond && pond) {
        shader.setVec3("objectColor", glm::vec3(0.05f, 0.3f, 0.55f));
        shader.setMat4("model", worldTransform * pondTransform);
        pond->draw();
    }

    // Bridge
    if (hasBridge && bridge) {
        GLuint woodTex = TextureLoader::load("textures/wood.png");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTex);
        shader.setInt("objectTexture", 0);
        shader.setVec3("objectColor", glm::vec3(0.6f, 0.4f, 0.2f));
        shader.setMat4("model", worldTransform * bridgeTransform);
        bridge->draw();
    }
}
