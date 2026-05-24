#pragma once

#include "HoleConfig.h"
#include <vector>

// ---------------------------------------------------------------------------
// Mock layout — temporary until Member 2 fills in real Vines data.
//
// Course bounds: 68.17 m (X) × 47.57 m (Z), centred at world origin.
// Two rows of 9, each hole a simple L-shape or rectangle.
//   Row 1 (holes  1-9) : Z centre = -15, runs left→right  (X = -28 … +28)
//   Row 2 (holes 10-18): Z centre = +15, runs right→left  (X = +28 … -28)
// Holes are 7 m apart along X; fairways are 12-15 m long, 3-4 m wide.
// ---------------------------------------------------------------------------

// ---- boundary-point helpers ------------------------------------------------

// Plain rectangle, centred at local origin, half-width hw, half-length hl
static inline std::vector<glm::vec2> rectBounds(float hw, float hl) {
    return {
        {-hw, -hl}, { hw, -hl}, { hw,  hl}, {-hw,  hl}
    };
}
static inline std::vector<float> flatElevs(int n, float y = 0.0f) {
    return std::vector<float>(static_cast<size_t>(n), y);
}

// L-shape: main rectangle plus a right-angle arm extending from +Z end
//   main : half-width mhw, runs from -mhl to +mhl
//   arm  : half-width ahw, runs from +mhl to +mhl+ahl, offset ox on X
// Returns 8 boundary points (CCW)
static inline std::vector<glm::vec2> lShapeBounds(
    float mhw, float mhl,
    float ahw, float ahl, float ox)
{
    float x0 = -mhw,  x1 = mhw;
    float z0 = -mhl,  z1 =  mhl;
    float ax0 = ox - ahw, ax1 = ox + ahw;
    float az1 = z1 + ahl;
    return {
        { x0,  z0},
        { x1,  z0},
        { x1,  z1},
        {ax1,  z1},
        {ax1, az1},
        {ax0, az1},
        {ax0,  z1},
        { x0,  z1},
    };
}
static inline std::vector<float> lShapeElevs(float bot, float top) {
    // 4 base points flat, 4 arm points rising toward top
    return {bot, bot, bot, bot, top, top, top, bot};
}

// ---- main builder ----------------------------------------------------------

inline std::vector<HoleConfig> buildCourseData() {
    std::vector<HoleConfig> holes;
    holes.reserve(18);

    // Row centres on Z; holes alternate slightly in Z to reduce visual overlap
    const float Z1 = -15.0f;
    const float Z2 =  15.0f;
    const float STEP = 7.0f;

    // Row 1 starts at X = -28, steps +7 each hole
    // Row 2 starts at X = +28, steps -7 each hole
    auto holeX = [&](int idx) -> float {
        int row = idx / 9, col = idx % 9;
        return (row == 0) ? (-28.0f + col * STEP)
                          : ( 28.0f - col * STEP);
    };
    auto holeZ = [&](int idx) -> float {
        return (idx / 9 == 0) ? Z1 : Z2;
    };

    int parCycle[4] = {3, 3, 4, 3};

    for (int i = 0; i < 18; ++i) {
        HoleConfig h;
        h.holeNumber = i + 1;
        h.par        = parCycle[i % 4];
        h.position   = glm::vec3(holeX(i), 0.0f, holeZ(i));
        h.rotation   = 0.0f;

        // Default: plain 3 m × 13 m rectangle, flat
        h.boundaryPoints = rectBounds(1.5f, 6.5f);
        h.elevations     = flatElevs(4);

        holes.push_back(h);
    }

    // ---- Per-hole overrides ------------------------------------------------
    // Shapes: odd-numbered holes get L-shapes; elevation changes every 3rd hole.
    // Features: stream holes 3, 7, 12; pond holes 5, 15; windmill hole 9.

    // --- Hole 1: wide rectangle, flat, par 3
    holes[0].boundaryPoints = rectBounds(2.0f, 7.0f);
    holes[0].elevations     = flatElevs(4);

    // --- Hole 2: L-shape, slight rise at arm, par 3
    holes[1].boundaryPoints = lShapeBounds(1.5f, 5.0f, 1.2f, 4.0f, 0.8f);
    holes[1].elevations     = lShapeElevs(0.0f, 0.8f);

    // --- Hole 3: rectangle + stream + bridge, par 4
    holes[2].boundaryPoints = rectBounds(1.8f, 7.0f);
    holes[2].elevations     = flatElevs(4);
    holes[2].hasStream = true;
    holes[2].hasBridge = true;
    holes[2].streamPath = { {0.0f, -4.0f}, {0.0f, -1.0f} };

    // --- Hole 4: L-shape, par 3
    holes[3].boundaryPoints = lShapeBounds(1.5f, 5.0f, 1.2f, 4.0f, -0.8f);
    holes[3].elevations     = lShapeElevs(0.0f, 1.0f);

    // --- Hole 5: rectangle + pond, par 4
    holes[4].boundaryPoints = rectBounds(2.0f, 7.5f);
    holes[4].elevations     = flatElevs(4);
    holes[4].hasPond        = true;
    holes[4].pondRadius     = 2.0f;

    // --- Hole 6: wide rectangle, par 3
    holes[5].boundaryPoints = rectBounds(2.0f, 6.0f);
    holes[5].elevations     = flatElevs(4);

    // --- Hole 7: L-shape + stream + bridge, par 4
    holes[6].boundaryPoints = lShapeBounds(1.5f, 5.0f, 1.2f, 3.5f, 0.7f);
    holes[6].elevations     = lShapeElevs(0.0f, 1.2f);
    holes[6].hasStream = true;
    holes[6].hasBridge = true;
    holes[6].streamPath = { {0.0f, -3.0f}, {0.0f, 0.0f} };

    // --- Hole 8: plain rectangle, flat, par 3
    holes[7].boundaryPoints = rectBounds(1.8f, 6.5f);
    holes[7].elevations     = flatElevs(4);

    // --- Hole 9: plain rectangle + windmill, par 4
    holes[8].boundaryPoints = rectBounds(1.8f, 7.0f);
    holes[8].elevations     = flatElevs(4);
    holes[8].hasWindmill     = true;
    holes[8].windmillLocalPos = glm::vec3(0.0f, 0.0f, 2.0f);

    // --- Hole 10: rectangle, slight elevation, par 3
    holes[9].boundaryPoints = rectBounds(1.8f, 6.5f);
    holes[9].elevations     = {0.0f, 0.0f, 1.5f, 1.5f};

    // --- Hole 11: L-shape, par 3
    holes[10].boundaryPoints = lShapeBounds(1.5f, 5.0f, 1.2f, 4.0f, 0.8f);
    holes[10].elevations     = lShapeElevs(0.0f, 1.0f);

    // --- Hole 12: rectangle + stream + bridge, par 4
    holes[11].boundaryPoints = rectBounds(2.0f, 7.5f);
    holes[11].elevations     = flatElevs(4);
    holes[11].hasStream = true;
    holes[11].hasBridge = true;
    holes[11].streamPath = { {0.0f, -5.0f}, {0.0f, -2.0f} };

    // --- Hole 13: L-shape, par 3
    holes[12].boundaryPoints = lShapeBounds(1.5f, 5.0f, 1.2f, 4.0f, -0.8f);
    holes[12].elevations     = lShapeElevs(0.0f, 1.5f);
    {
        HoleConfig::ObstacleEntry e1;
        e1.type = "Barrel"; e1.localPos = glm::vec3(-1.0f, 0.35f, 0.5f);
        e1.scale = glm::vec3(1.0f); e1.rotation = 0.0f;
        holes[12].obstacles.push_back(e1);

        HoleConfig::ObstacleEntry e2;
        e2.type = "Barrel"; e2.localPos = glm::vec3( 1.0f, 0.35f, 0.5f);
        e2.scale = glm::vec3(1.0f); e2.rotation = 45.0f;
        holes[12].obstacles.push_back(e2);
    }

    // --- Hole 14: plain rectangle, par 3
    holes[13].boundaryPoints = rectBounds(1.8f, 6.5f);
    holes[13].elevations     = flatElevs(4);

    // --- Hole 15: wide rectangle + pond, par 4
    holes[14].boundaryPoints = rectBounds(2.2f, 7.5f);
    holes[14].elevations     = flatElevs(4);
    holes[14].hasPond        = true;
    holes[14].pondRadius     = 1.8f;

    // --- Hole 16: L-shape, elevation rise, par 3
    holes[15].boundaryPoints = lShapeBounds(1.5f, 5.0f, 1.2f, 3.5f, 0.7f);
    holes[15].elevations     = lShapeElevs(0.0f, 1.3f);

    // --- Hole 17: plain rectangle, par 3
    holes[16].boundaryPoints = rectBounds(1.8f, 7.0f);
    holes[16].elevations     = flatElevs(4);

    // --- Hole 18: wide rectangle, par 4, flag decor, tunnel
    holes[17].par            = 4;
    holes[17].boundaryPoints = rectBounds(2.2f, 8.0f);
    holes[17].elevations     = {0.0f, 0.0f, 1.5f, 1.5f};
    holes[17].hasTunnel      = true;
    {
        HoleConfig::ObstacleEntry e;
        e.type = "FlagPole"; e.localPos = glm::vec3(0.0f, 0.0f, -5.5f);
        e.scale = glm::vec3(1.0f); e.rotation = 0.0f;
        holes[17].decor.push_back(e);
    }

    return holes;
}
