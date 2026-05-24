#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole17() {
    HoleConfig cfg;
    cfg.holeNumber = 17;
    cfg.par        = 3;
    cfg.position   = glm::vec3(23.0f, 0.5f, 24.0f);
    setRectHole(cfg, 16.92f, 5.00f, 110.63f, {0.05f, 0.08f, 0.10f, 0.06f});
    cfg.hasStream  = true;
    cfg.streamPath = {{2.05f, -5.6f}, {2.05f, 5.6f}};
    cfg.obstacles.push_back(obs("Rock",  -1.0f, 0.45f,  0.4f, 1.5f, 1.0f, 1.5f));
    cfg.obstacles.push_back(obs("Plank",  1.3f, 0.10f, -0.6f, 1.4f, 1.0f, 0.8f, 5.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 5.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
