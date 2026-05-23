#pragma once

#include "HoleConfig.h"
#include <vector>

// Course bounds: 68.17 m (X) × 47.57 m (Z), centred at world origin.
// Holes are arranged in two rows of 9 across the course.
// Member 2 will replace the placeholder boundaryPoints with real geometry.

inline std::vector<HoleConfig> buildCourseData() {
    std::vector<HoleConfig> holes;
    holes.reserve(18);

    // Row 1 — holes 1-9, Z ≈ -12  (left to right along X)
    // Row 2 — holes 10-18, Z ≈ +12 (right to left along X)
    const float rowZ[2]  = { -12.0f,  12.0f };
    const float startX   = -30.0f;
    const float stepX    =   7.5f;

    // Placeholder boundary: small rectangle around local origin
    auto stubBoundary = []() -> std::vector<glm::vec2> {
        return {
            {-2.0f, -3.5f},
            { 2.0f, -3.5f},
            { 2.0f,  3.5f},
            {-2.0f,  3.5f},
        };
    };
    auto stubElevations = []() -> std::vector<float> {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    };

    int parCycle[4] = {3, 4, 3, 5};   // rough par rotation

    for (int i = 0; i < 18; ++i) {
        HoleConfig h;
        h.holeNumber = i + 1;
        h.par        = parCycle[i % 4];

        int row = i / 9;
        int col = i % 9;
        float xSign = (row == 0) ? 1.0f : -1.0f;   // row 2 runs back right-to-left
        h.position = glm::vec3(startX + col * stepX * xSign,
                               0.0f,
                               rowZ[row]);
        h.rotation = 0.0f;

        h.boundaryPoints = stubBoundary();
        h.elevations     = stubElevations();

        holes.push_back(h);
    }

    // ---- Individual overrides (Member 2 will expand these) -----------------

    // Hole 1 — straight par-3, slight left dogleg
    holes[0].par      = 3;
    holes[0].rotation = 10.0f;

    // Hole 3 — has a pond
    holes[2].hasPond   = true;
    holes[2].pondRadius = 1.8f;

    // Hole 5 — has a stream with bridge
    holes[4].hasStream = true;
    holes[4].hasBridge = true;
    holes[4].streamPath = { {0.0f, -1.0f}, {0.0f, 1.0f} };

    // Hole 9 — windmill obstacle
    holes[8].hasWindmill     = true;
    holes[8].windmillLocalPos = glm::vec3(0.0f, 0.0f, 0.5f);

    // Hole 13 — barrel obstacles
    holes[12].obstacles.push_back({"Barrel", glm::vec3(-1.0f, 0.35f, 0.0f),
                                   glm::vec3(1.0f), 0.0f});
    holes[12].obstacles.push_back({"Barrel", glm::vec3( 1.0f, 0.35f, 0.0f),
                                   glm::vec3(1.0f), 45.0f});

    // Hole 18 — finishing hole, par 4, has tunnel flag
    holes[17].par       = 4;
    holes[17].hasTunnel = true;
    holes[17].decor.push_back({"FlagPole", glm::vec3(0.0f, 0.0f, -2.5f),
                               glm::vec3(1.0f), 0.0f});

    return holes;
}
