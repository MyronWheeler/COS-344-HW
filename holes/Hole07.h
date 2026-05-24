#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole07() {
    HoleConfig cfg;
    cfg.holeNumber = 7;
    cfg.par = 4;
    cfg.position = glm::vec3(38.0f, 0.5f, 0.0f);
    setRectHole(cfg, 12.67f, 4.84f, 70.36f, {0.00f, 0.15f, 0.15f, 0.00f});
    cfg.hasStream = true;
    cfg.hasBridge = true;
    cfg.streamPath = {{1.85f, -4.9f}, {1.85f, 4.9f}};
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 4.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
