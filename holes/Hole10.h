#pragma once
#include "../HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole10() {
    HoleConfig cfg;
    cfg.holeNumber = 10;
    cfg.par = 3;
    cfg.position = glm::vec3(-8.0f, 0.5f, 2.0f);
    setRectHole(cfg, 12.41f, 3.56f, 289.59f, {0.00f, 0.00f, 0.40f, 0.40f});
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 4.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
