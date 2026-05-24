#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "HoleConfig.h"
#include "Terrain.h"
#include "Mesh.h"

class Shader;

// Move-only pairing of a Mesh and its local transform — replaces std::pair
struct StreamSegment {
    Mesh      mesh;
    glm::mat4 transform;

    StreamSegment(Mesh &&m, glm::mat4 t) : mesh(std::move(m)), transform(t) {}
    StreamSegment(StreamSegment &&)                 = default;
    StreamSegment &operator=(StreamSegment &&)      = default;
    StreamSegment(const StreamSegment &)            = delete;
    StreamSegment &operator=(const StreamSegment &) = delete;
};

struct HoleNode {
    glm::mat4 worldTransform;

    Terrain  *terrain  = nullptr;
    Mesh     *pond     = nullptr;
    Mesh     *bridge   = nullptr;
    glm::mat4 pondTransform;
    glm::mat4 bridgeTransform;

    std::vector<StreamSegment> streamSegments;

    struct PlacedObject {
        std::string type;
        glm::mat4   transform;
    };
    std::vector<PlacedObject> obstacles;
    std::vector<PlacedObject> decor;

    bool hasPond   = false;
    bool hasBridge = false;

    void draw(Shader &shader);

    HoleNode()  = default;
    ~HoleNode() { delete terrain; delete pond; delete bridge; }

    // Move constructor — transfers raw-pointer ownership and nulls the source
    HoleNode(HoleNode &&o)
        : worldTransform(o.worldTransform)
        , terrain(o.terrain)
        , pond(o.pond)
        , bridge(o.bridge)
        , pondTransform(o.pondTransform)
        , bridgeTransform(o.bridgeTransform)
        , streamSegments(std::move(o.streamSegments))
        , obstacles(std::move(o.obstacles))
        , decor(std::move(o.decor))
        , hasPond(o.hasPond)
        , hasBridge(o.hasBridge)
    {
        o.terrain = nullptr;
        o.pond    = nullptr;
        o.bridge  = nullptr;
    }

    HoleNode &operator=(HoleNode &&o) {
        if (this != &o) {
            delete terrain; delete pond; delete bridge;
            worldTransform  = o.worldTransform;
            terrain         = o.terrain;  o.terrain = nullptr;
            pond            = o.pond;     o.pond    = nullptr;
            bridge          = o.bridge;   o.bridge  = nullptr;
            pondTransform   = o.pondTransform;
            bridgeTransform = o.bridgeTransform;
            streamSegments  = std::move(o.streamSegments);
            obstacles       = std::move(o.obstacles);
            decor           = std::move(o.decor);
            hasPond         = o.hasPond;
            hasBridge       = o.hasBridge;
        }
        return *this;
    }

    HoleNode(const HoleNode &)            = delete;
    HoleNode &operator=(const HoleNode &) = delete;
};

class HoleFactory {
public:
    static HoleNode build(const HoleConfig &config);
};
