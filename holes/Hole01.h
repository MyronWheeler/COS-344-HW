#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole01() {
    HoleConfig cfg;
    cfg.holeNumber = 1;
    cfg.par = 3;
    cfg.position = glm::vec3(-38.0f, 0.5f, -24.0f);
    setRectHole(cfg, 15.69f, 4.55f, 115.00f, {0.55f, 0.55f, 0.15f, 0.15f});
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 5.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
