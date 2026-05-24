#include "HoleFactory.h"
#include "Shader.h"
#include "TextureLoader.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// Build one stream-segment plane between two waypoints
static StreamSegment makeStreamSegment(glm::vec2 a, glm::vec2 b, float width) {
    glm::vec2 dir = b - a;
    float     len = glm::length(dir);
    if (len < 0.001f) len = 0.001f;

    std::vector<Vertex> verts = {
        {{-0.5f, 0.0f, -0.5f}, {0,1,0}, {0,0}},
        {{ 0.5f, 0.0f, -0.5f}, {0,1,0}, {1,0}},
        {{ 0.5f, 0.0f,  0.5f}, {0,1,0}, {1,1}},
        {{-0.5f, 0.0f,  0.5f}, {0,1,0}, {0,1}},
    };
    std::vector<unsigned int> idx = {0,1,2, 0,2,3};
    Mesh seg(verts, idx);

    glm::vec3 mid((a.x + b.x) * 0.5f, 0.01f, (a.y + b.y) * 0.5f);
    float angle = std::atan2(dir.x, dir.y);
    glm::mat4 t = glm::translate(glm::mat4(1.0f), mid);
    t = glm::rotate(t, angle, glm::vec3(0, 1, 0));
    t = glm::scale(t, glm::vec3(width, 1.0f, len));

    return StreamSegment(std::move(seg), t);
}

HoleNode HoleFactory::build(const HoleConfig &cfg) {
    HoleNode node;

    node.worldTransform = glm::translate(glm::mat4(1.0f), cfg.position);
    node.worldTransform = glm::rotate(node.worldTransform,
                                      glm::radians(cfg.rotation),
                                      glm::vec3(0, 1, 0));

    if (cfg.boundaryPoints.size() >= 3) {
        std::vector<float> elevs = cfg.elevations;
        while (elevs.size() < cfg.boundaryPoints.size()) elevs.push_back(0.0f);
        node.terrain = new Terrain(cfg.boundaryPoints, elevs);
    }

    if (cfg.hasStream && cfg.streamPath.size() >= 2) {
        for (size_t i = 0; i + 1 < cfg.streamPath.size(); ++i)
            node.streamSegments.push_back(
                makeStreamSegment(cfg.streamPath[i], cfg.streamPath[i + 1], 1.5f));
    }

    if (cfg.hasPond) {
        node.hasPond       = true;
        node.pond          = new Mesh(Mesh::createSphere(cfg.pondRadius, 32));
        node.pondTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.02f, 0))
                           * glm::scale(glm::mat4(1.0f), glm::vec3(1, 0.05f, 1));
    }

    if (cfg.hasBridge && cfg.streamPath.size() >= 2) {
        node.hasBridge      = true;
        node.bridge         = new Mesh(Mesh::createBox(2.0f, 0.1f, 1.6f));
        glm::vec2 mid       = (cfg.streamPath[0] + cfg.streamPath[1]) * 0.5f;
        glm::vec2 dir       = cfg.streamPath[1] - cfg.streamPath[0];
        float angle         = std::atan2(dir.x, dir.y);
        node.bridgeTransform = glm::rotate(
                                 glm::translate(glm::mat4(1.0f), glm::vec3(mid.x, 0.06f, mid.y)),
                                 angle, glm::vec3(0, 1, 0));
    }

    for (const auto &e : cfg.obstacles) {
        HoleNode::PlacedObject po;
        po.type      = e.type;
        po.transform = glm::scale(
                         glm::rotate(
                           glm::translate(glm::mat4(1.0f), e.localPos),
                           glm::radians(e.rotation), glm::vec3(0,1,0)),
                         e.scale);
        node.obstacles.push_back(std::move(po));
    }

    for (const auto &e : cfg.decor) {
        HoleNode::PlacedObject po;
        po.type      = e.type;
        po.transform = glm::scale(
                         glm::rotate(
                           glm::translate(glm::mat4(1.0f), e.localPos),
                           glm::radians(e.rotation), glm::vec3(0,1,0)),
                         e.scale);
        node.decor.push_back(std::move(po));
    }

    return node;
}

void HoleNode::draw(Shader &shader) {
    if (terrain)
        terrain->draw(shader, worldTransform);

    {
        GLuint waterTex = TextureLoader::load("textures/water_normal.png");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTex);
        shader.setInt("objectTexture", 0);
        shader.setVec3("objectColor", glm::vec3(0.1f, 0.35f, 0.5f));
        for (auto &seg : streamSegments) {
            shader.setMat4("model", worldTransform * seg.transform);
            seg.mesh.draw();
        }
    }

    if (hasPond && pond) {
        shader.setVec3("objectColor", glm::vec3(0.05f, 0.3f, 0.55f));
        shader.setMat4("model", worldTransform * pondTransform);
        pond->draw();
    }

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
