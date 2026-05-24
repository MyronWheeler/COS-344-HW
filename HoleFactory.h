#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "HoleConfig.h"
#include "Terrain.h"
#include "Mesh.h"
#include "Objects.h"

class Shader;

struct StreamSegment {
    Mesh mesh;
    glm::mat4 transform;

    StreamSegment(Mesh &&m, glm::mat4 t) : mesh(std::move(m)), transform(t) {}
    StreamSegment(StreamSegment &&) = default;
    StreamSegment &operator=(StreamSegment &&) = default;
    StreamSegment(const StreamSegment &) = delete;
    StreamSegment &operator=(const StreamSegment &) = delete;
};

struct HoleNode {
    glm::mat4 worldTransform;

    Terrain* terrain = nullptr;
    Mesh* bridge = nullptr;
    Windmill* windmill = nullptr;
    glm::mat4 bridgeTransform;
    glm::mat4 windmillTransform;

    std::vector<StreamSegment> streamSegments;

    struct PlacedObject {
        std::string type;
        glm::mat4   transform;
    };
    std::vector<PlacedObject> obstacles;
    std::vector<PlacedObject> decor;

    struct HoleCup {
        glm::mat4 transform;
    };
    std::vector<HoleCup> holeCups;

    bool hasBridge = false;
    bool hasWindmill = false;

    void draw(Shader &shader, float spinAngle = 0.0f);

    HoleNode() = default;
    ~HoleNode() { 
        delete terrain; 
        delete bridge; 
        delete windmill;
    }

    HoleNode(HoleNode &&o)
        : worldTransform(o.worldTransform)
        , terrain(o.terrain)
        , bridge(o.bridge)
        , windmill(o.windmill)
        , bridgeTransform(o.bridgeTransform)
        , windmillTransform(o.windmillTransform)
        , streamSegments(std::move(o.streamSegments))
        , obstacles(std::move(o.obstacles))
        , decor(std::move(o.decor))
        , holeCups(std::move(o.holeCups))
        , hasBridge(o.hasBridge)
        , hasWindmill(o.hasWindmill)
    {
        o.terrain  = nullptr;
        o.bridge   = nullptr;
        o.windmill = nullptr;
    }

    HoleNode &operator=(HoleNode &&o) {
        if (this != &o) {
            delete terrain; 
            delete bridge; 
            delete windmill;
            worldTransform = o.worldTransform;
            terrain = o.terrain;   
            o.terrain  = nullptr;
            bridge = o.bridge; 
            o.bridge = nullptr;
            windmill = o.windmill;
            o.windmill = nullptr;
            bridgeTransform = o.bridgeTransform;
            windmillTransform = o.windmillTransform;
            streamSegments = std::move(o.streamSegments);
            obstacles = std::move(o.obstacles);
            decor = std::move(o.decor);
            holeCups = std::move(o.holeCups);
            hasBridge = o.hasBridge;
            hasWindmill = o.hasWindmill;
        }
        return* this;
    }

    HoleNode(const HoleNode &) = delete;
    HoleNode &operator=(const HoleNode &) = delete;
}; 

class HoleFactory {
public:
    static HoleNode build(const HoleConfig &config);
};
