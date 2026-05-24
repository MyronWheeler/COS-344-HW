#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole10() {
    HoleConfig cfg;
    cfg.holeNumber = 10;
    cfg.par        = 3;
    cfg.position   = glm::vec3(-4.0f, 0.5f, 2.0f);
    setRectHole(cfg, 12.41f, 3.56f, 289.59f, {0.00f, 0.00f, 0.40f, 0.40f});
    return cfg;
}
