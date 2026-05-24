#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole09() {
    HoleConfig cfg;
    cfg.holeNumber       = 9;
    cfg.par              = 3;
    cfg.position         = glm::vec3(8.0f, 0.5f, 0.0f);
    setRectHole(cfg, 11.35f, 5.00f, 57.04f, {0.45f, 0.10f, 0.00f, 0.00f});
    cfg.hasWindmill      = true;
    cfg.windmillLocalPos = glm::vec3(0.4f, 0.0f, 1.8f);
    return cfg;
}
