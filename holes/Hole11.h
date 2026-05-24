#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole11() {
    HoleConfig cfg;
    cfg.holeNumber = 11;
    cfg.par = 4;
    cfg.position = glm::vec3(-23.0f, 0.5f, 0.0f);
    setRectHole(cfg, 9.88f, 5.70f, 176.87f, {0.00f, 0.15f, 0.45f, 0.10f});
    cfg.obstacles.push_back(obs("Barrel", 0.0f, 0.35f, 0.6f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 3.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
