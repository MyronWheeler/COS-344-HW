#pragma once
#include "../HoleConfig.h"
#include "HoleHelpers.h"

inline HoleConfig buildHole12() {
    HoleConfig cfg;
    cfg.holeNumber = 12;
    cfg.par        = 3;
    cfg.position   = glm::vec3(-38.0f, 0.5f, 2.0f);
    setRectHole(cfg, 12.76f, 6.03f, 132.90f, {0.15f, 0.40f, 0.40f, 0.15f});
    cfg.hasStream  = true;
    cfg.hasBridge  = true;
    cfg.streamPath = {{0.0f, -5.0f}, {0.0f, 5.0f}};
    cfg.obstacles.push_back(obs("Rock", -1.6f, 0.35f, -1.2f, 1.0f, 1.0f, 1.0f));
    cfg.obstacles.push_back(obs("Rock",  0.2f, 0.35f,  0.3f, 0.9f, 0.9f, 0.9f));
    cfg.obstacles.push_back(obs("Rock",  1.5f, 0.35f,  1.2f, 1.0f, 1.0f, 1.0f));
    cfg.decor.push_back(obs("FlagPole", 0.0f, 0.0f, 4.5f, 0.7f, 1.2f, 0.7f));
    return cfg;
}
