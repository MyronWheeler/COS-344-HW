#pragma once
#include "../HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole03() {
    HoleConfig cfg;
    cfg.holeNumber = 3;
    cfg.par        = 4;
    cfg.position   = glm::vec3(-8.0f, 0.5f, -24.0f);
    setRectHole(cfg, 9.76f, 3.93f, 210.80f, {0.00f, 0.20f, 0.35f, 0.10f});
    cfg.hasStream  = true;
    cfg.hasBridge  = true;
    cfg.streamPath = {{-1.45f, -4.4f}, {-1.45f, 4.4f}};
    cfg.obstacles.push_back(obs("Bunker", 0.0f, 0.0f, 3.5f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 3.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
