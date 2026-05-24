#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole06() {
    HoleConfig cfg;
    cfg.holeNumber = 6;
    cfg.par = 3;
    cfg.position = glm::vec3(38.0f, 0.5f, -26.0f);
    setRectHole(cfg, 9.33f, 6.02f, 25.23f, {0.00f, 0.45f, 0.45f, 0.00f});
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 3.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
