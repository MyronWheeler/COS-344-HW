#pragma once
#include "HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole18() {
    HoleConfig cfg;
    cfg.holeNumber      = 18;
    cfg.par             = 4;
    cfg.position        = glm::vec3(38.0f, 0.5f, 26.0f);
    cfg.boundaryPoints  = ellipseBounds(3.60f, 2.405f, 12);
    cfg.elevations      = flatElevs(12);
    cfg.rotation        = 54.64f;
    cfg.hasTunnel       = true;
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, -5.5f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f,  1.8f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
