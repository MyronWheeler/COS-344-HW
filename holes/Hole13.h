#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole13() {
    HoleConfig cfg;
    cfg.holeNumber = 13;
    cfg.par        = 3;
    cfg.position   = glm::vec3(-38.0f, 0.5f, 24.0f);
    setRectHole(cfg, 11.77f, 4.16f, 144.09f, {0.35f, 0.25f, 0.20f, 0.40f});
    cfg.obstacles.push_back(obs("Bunker", -1.4f, 0.0f,  0.7f, 0.9f,  0.9f,  0.9f));
    cfg.obstacles.push_back(obs("Rock",    1.3f, 0.35f, -0.2f, 0.85f, 0.85f, 0.85f, 20.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 4.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
