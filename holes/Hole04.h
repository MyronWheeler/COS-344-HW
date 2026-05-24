#pragma once
#include "../HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole04() {
    HoleConfig cfg;
    cfg.holeNumber = 4;
    cfg.par        = 3;
    cfg.position   = glm::vec3(8.0f, 0.5f, -26.0f);
    setRectHole(cfg, 15.00f, 3.35f, 109.35f, {0.40f, 0.20f, 0.00f, 0.15f});
    cfg.obstacles.push_back(obs("Bunker", 0.0f, 0.0f, 5.5f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 5.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
