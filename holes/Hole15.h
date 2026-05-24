#pragma once
#include "../HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole15() {
    HoleConfig cfg;
    cfg.holeNumber = 15;
    cfg.par = 4;
    cfg.position = glm::vec3(-8.0f, 0.5f, 24.0f);
    setRectHole(cfg, 18.24f, 4.42f, 231.82f, {0.00f, 0.15f, 0.35f, 0.25f});
    cfg.hasPond = true;
    cfg.pondRadius = 1.8f;
    cfg.obstacles.push_back(obs("Bunker", -1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 6.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
