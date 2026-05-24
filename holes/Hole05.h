#pragma once
#include "../HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole05() {
    HoleConfig cfg;
    cfg.holeNumber = 5;
    cfg.par = 3;
    cfg.position = glm::vec3(23.0f, 0.5f, -24.0f);
    setRectHole(cfg, 15.51f, 5.83f, 65.10f, {0.00f, 0.50f, 0.35f, 0.10f});
    cfg.hasPond = true;
    cfg.pondRadius = 2.0f;
    cfg.obstacles.push_back(obs("Rock",  -1.6f, 0.25f, -1.0f, 1.0f, 1.0f, 1.0f));
    cfg.obstacles.push_back(obs("Plank",  1.4f, 0.10f,  0.8f, 1.0f, 1.0f, 1.8f, 15.0f));
    cfg.obstacles.push_back(obs("Bunker", 0.0f, 0.0f, 5.5f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 5.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
