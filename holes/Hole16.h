#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole16() {
    HoleConfig cfg;
    cfg.holeNumber = 16;
    cfg.par = 3;
    cfg.position = glm::vec3(8.0f, 0.5f, 26.0f);
    setRectHole(cfg, 15.69f, 4.15f, 130.68f, {0.30f, 0.05f, 0.00f, 0.25f});
    cfg.hasStream = true;
    cfg.streamPath = {{0.0f, -5.2f}, {0.0f, 5.2f}};
    cfg.obstacles.push_back(obs("Bunker", 1.35f, 0.0f, -0.8f, 0.85f, 0.85f, 0.85f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 5.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
