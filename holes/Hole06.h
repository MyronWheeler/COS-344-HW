#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole06() {
    HoleConfig cfg;
    cfg.holeNumber = 6;
    cfg.par        = 3;
    cfg.position   = glm::vec3(28.0f, 0.5f, -22.0f);
    setRectHole(cfg, 9.33f, 6.02f, 25.23f, {0.00f, 0.45f, 0.45f, 0.00f});
    return cfg;
}
