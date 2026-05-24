#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole01() {
    HoleConfig cfg;
    cfg.holeNumber = 1;
    cfg.par        = 3;
    cfg.position   = glm::vec3(-32.0f, 0.5f, -20.0f);
    setRectHole(cfg, 15.69f, 4.55f, 115.00f, {0.55f, 0.55f, 0.15f, 0.15f});
    return cfg;
}
