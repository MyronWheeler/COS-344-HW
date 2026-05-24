#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole02() {
    HoleConfig cfg;
    cfg.holeNumber = 2;
    cfg.par        = 3;
    cfg.position   = glm::vec3(-20.0f, 0.5f, -22.0f);
    setRectHole(cfg, 12.11f, 3.58f, 224.04f, {0.45f, 0.45f, 0.00f, 0.00f});
    return cfg;
}
