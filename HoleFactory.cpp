#include "HoleFactory.h"
#include "Shader.h"
#include "TextureLoader.h"
#include "MathHelpers.h"
#include <GL/glew.h>
#include <cmath>


static StreamSegment makeStreamSegment(glm::vec2 a, glm::vec2 b, float width) {
    glm::vec2 dir = b - a;
    float len = glm::length(dir);
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
    glm::mat4 t = makeTranslate(glm::vec3(mid.x, 0.01f, mid.y))
                * makeRotate(angle, glm::vec3(0, 1, 0))
                * makeScale(glm::vec3(width, 1.0f, len));

    return StreamSegment(std::move(seg), t);
}

HoleNode HoleFactory::build(const HoleConfig &cfg) {
    HoleNode node;

    node.worldTransform = makeTranslate(cfg.position)
                        * makeRotate(degToRad(cfg.rotation), glm::vec3(0, 1, 0));

    if (cfg.boundaryPoints.size() >= 3) {
        std::vector<float> elevs = cfg.elevations;
        while (elevs.size() < cfg.boundaryPoints.size()) elevs.push_back(0.0f);
        node.terrain = new Terrain(cfg.boundaryPoints, elevs);
    }

    
    

    if (cfg.hasBridge && cfg.streamPath.size() >= 2) {
        node.hasBridge = true;
        node.bridge = new Mesh(Mesh::createBox(2.0f, 0.1f, 1.6f));
        glm::vec2 mid = (cfg.streamPath[0] + cfg.streamPath[1]) * 0.5f;
        glm::vec2 dir = cfg.streamPath[1] - cfg.streamPath[0];
        float angle = std::atan2(dir.x, dir.y);
        node.bridgeTransform = makeTranslate(glm::vec3(mid.x, 0.06f, mid.y))
                     * makeRotate(angle, glm::vec3(0, 1, 0));
    }

    if (cfg.hasWindmill) {
        node.hasWindmill= true;
        node.windmill  = new Windmill();
        node.windmillTransform = makeTranslate(cfg.windmillLocalPos);
    }

    
    
    glm::vec2 holeCentroid(0.0f, 0.0f);
    if (!cfg.boundaryPoints.empty()) {
        for (const auto &p : cfg.boundaryPoints) holeCentroid += p;
        holeCentroid /= static_cast<float>(cfg.boundaryPoints.size());
    }

    for (const auto &e : cfg.obstacles) {
        HoleNode::PlacedObject po;
        po.type  = e.type;
        glm::vec3 pos = e.localPos;
        if (e.type == "Bunker") {
            
            
            if (!cfg.boundaryPoints.empty()) {
                pos.x = holeCentroid.x;
                pos.z = holeCentroid.y;
            }
            pos.y = 0.18f; 
        }
        po.transform  = makeTranslate(pos)
                  * makeRotate(degToRad(e.rotation), glm::vec3(0,1,0))
                  * makeScale(e.scale);
        node.obstacles.push_back(std::move(po));
    }

    for (const auto &e : cfg.decor) {
        HoleNode::PlacedObject po;
        po.type      = e.type;
                        po.transform = makeTranslate(e.localPos)
                                                 * makeRotate(degToRad(e.rotation), glm::vec3(0,1,0))
                                                 * makeScale(e.scale);
        node.decor.push_back(std::move(po));

        if (e.type == "FlagPole") {
            HoleNode::HoleCup cup;
            cup.transform = makeTranslate(glm::vec3(e.localPos.x, 0.08f, e.localPos.z));
            node.holeCups.push_back(cup);
        }
    }

    return node;
}

void HoleNode::draw(Shader &shader, float spinAngle) {
    if (terrain)
        terrain->draw(shader, worldTransform);

    

    if (hasBridge && bridge) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/wood.png"));
        shader.setInt("objectTexture", 0);
        shader.setInt("useTexture", 1);
        shader.setMat4("model", worldTransform * bridgeTransform);
        bridge->draw();
        shader.setInt("useTexture", 0);
    }

    if (hasWindmill && windmill)
        windmill->draw(shader, worldTransform * windmillTransform, spinAngle);

    
    {
        static Mesh s_rock = Mesh::createSphere(0.5f, 12);
        static Mesh s_barrel = Mesh::createCylinder(0.35f, 0.7f, 16);
        static Mesh s_bunker = Mesh::createCylinder(0.9f, 0.04f, 16);
        static Mesh s_plank  = Mesh::createBox(2.0f, 0.08f, 0.6f);
        static Mesh s_fallback = Mesh::createBox(0.5f, 0.5f, 0.5f);
        static FlagPole s_flag;

        for (int pass = 0; pass < 2; ++pass) {
            const std::vector<PlacedObject> &list = (pass == 0) ? obstacles : decor;
            for (const auto &po : list) {
                glm::mat4 m = worldTransform * po.transform;
                if (po.type == "Rock") {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/rock.png"));
                    shader.setInt("objectTexture", 0);
                    shader.setInt("useTexture", 1);
                    shader.setMat4("model", m);
                    s_rock.draw();
                    shader.setInt("useTexture", 0);
                } else if (po.type == "Barrel") {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/wood.png"));
                    shader.setInt("objectTexture", 0);
                    shader.setInt("useTexture", 1);
                    shader.setMat4("model", m);
                    s_barrel.draw();
                    shader.setInt("useTexture", 0);
                } else if (po.type == "Bunker") {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/sand.png"));
                    shader.setInt("objectTexture", 0);
                    shader.setInt("useTexture", 1);
                    shader.setMat4("model", m);
                    s_bunker.draw();
                    shader.setInt("useTexture", 0);
                } else if (po.type == "Plank") {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/wood.png"));
                    shader.setInt("objectTexture", 0);
                    shader.setInt("useTexture", 1);
                    shader.setMat4("model", m);
                    s_plank.draw();
                    shader.setInt("useTexture", 0);
                } else if (po.type == "FlagPole") {
                    s_flag.draw(shader, m);
                } else {
                    shader.setInt("useTexture", 0);
                    shader.setVec3("objectColor", glm::vec3(0.50f, 0.50f, 0.50f));
                    shader.setMat4("model", m);
                    s_fallback.draw();
                }
            }
        }
    }

    
    {
        static Mesh cupMesh = Mesh::createCylinder(0.25f, 0.02f, 16);
        shader.setInt("useTexture", 0);
        shader.setVec3("objectColor", glm::vec3(0.0f, 0.0f, 0.0f));
        for (auto &cup : holeCups) {
            shader.setMat4("model", worldTransform * cup.transform);
            cupMesh.draw();
        }
    }
}
