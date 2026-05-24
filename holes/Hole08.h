#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole08() {
    HoleConfig cfg;
    cfg.holeNumber = 8;
    cfg.par        = 3;
    cfg.position   = glm::vec3(23.0f, 0.5f, 2.0f);
    setRectHole(cfg, 12.14f, 4.49f, 42.52f, {0.00f, 0.35f, 0.60f, 0.20f});
    cfg.obstacles.push_back(obs("Bunker", -1.2f, 0.0f, -1.5f, 0.9f, 0.9f, 0.9f));
    cfg.obstacles.push_back(obs("Bunker",  0.9f, 0.0f,  0.0f, 0.8f, 0.8f, 0.8f));
    cfg.obstacles.push_back(obs("Bunker", -0.1f, 0.0f,  1.6f, 0.85f, 0.85f, 0.85f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 4.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
