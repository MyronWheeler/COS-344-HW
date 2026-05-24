#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole14() {
    HoleConfig cfg;
    cfg.holeNumber = 14;
    cfg.par        = 3;
    cfg.position   = glm::vec3(-20.0f, 0.5f, 22.0f);
    setRectHole(cfg, 8.72f, 4.00f, 124.55f, {0.45f, 0.10f, 0.00f, 0.00f});
    return cfg;
}
