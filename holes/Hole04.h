#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole04() {
    HoleConfig cfg;
    cfg.holeNumber = 4;
    cfg.par        = 3;
    cfg.position   = glm::vec3(4.0f, 0.5f, -22.0f);
    setRectHole(cfg, 15.00f, 3.35f, 109.35f, {0.40f, 0.20f, 0.00f, 0.15f});
    return cfg;
}
