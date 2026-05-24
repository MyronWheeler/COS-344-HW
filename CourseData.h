#pragma once

#include "HoleConfig.h"
#include <cmath>
#include <vector>

// ---------------------------------------------------------------------------
// Research-based layout data for the Vines mini golf course.
//
// The measured hole dimensions from the spreadsheet are encoded here as local
// boundary polygons and elevation samples. Because HoleConfig stores a 2-D
// footprint rather than a traced satellite outline, each fairway is
// approximated with a rotated rectangle or ellipse using the recorded length
// and width.
// ---------------------------------------------------------------------------

// ---- boundary-point helpers ------------------------------------------------

// Plain rectangle, centred at local origin, half-width hw, half-length hl
static inline std::vector<glm::vec2> rectBounds(float hw, float hl) {
    return {
        {-hw, -hl}, { hw, -hl}, { hw,  hl}, {-hw,  hl}
    };
}
static inline std::vector<glm::vec2> rectBoundsFromSize(float length, float width) {
    return rectBounds(width * 0.5f, length * 0.5f);
}
static inline std::vector<glm::vec2> ellipseBounds(float rx, float rz, int segments = 12) {
    std::vector<glm::vec2> points;
    points.reserve(static_cast<size_t>(segments));
    const float tau = 6.28318530718f;
    for (int i = 0; i < segments; ++i) {
        float t = tau * static_cast<float>(i) / static_cast<float>(segments);
        points.push_back({rx * std::cos(t), rz * std::sin(t)});
    }
    return points;
}
static inline std::vector<float> flatElevs(int n, float y = 0.0f) {
    return std::vector<float>(static_cast<size_t>(n), y);
}

static inline void setRectHole(HoleConfig &hole,
                               float length,
                               float width,
                               float heading,
                               std::vector<float> elevations)
{
    hole.boundaryPoints = rectBoundsFromSize(length, width);
    hole.elevations     = std::move(elevations);
    hole.rotation       = heading;
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

        // Default footprint is overwritten below with the measured research data.
        h.boundaryPoints = rectBoundsFromSize(13.0f, 3.0f);
        h.elevations     = flatElevs(4);

        holes.push_back(h);
    }

    // ---- Per-hole overrides ------------------------------------------------
    // The spreadsheet gives measured footprints rather than exact traced paths,
    // so the polygons below use the recorded dimensions as rotated rectangles.

    // --- Hole 1
    setRectHole(holes[0], 15.69f, 4.55f, 115.00f, {0.55f, 0.55f, 0.15f, 0.15f});

    // --- Hole 2
    setRectHole(holes[1], 12.11f, 3.58f, 224.04f, {0.45f, 0.45f, 0.00f, 0.00f});

    // --- Hole 3
    setRectHole(holes[2], 9.76f, 3.93f, 210.80f, {0.00f, 0.20f, 0.35f, 0.10f});
    holes[2].hasStream = true;
    holes[2].hasBridge = true;
    holes[2].streamPath = {{-1.45f, -4.4f}, {-1.45f, 4.4f}};

    // --- Hole 4
    setRectHole(holes[3], 15.00f, 3.35f, 109.35f, {0.40f, 0.20f, 0.00f, 0.15f});

    // --- Hole 5
    setRectHole(holes[4], 15.51f, 5.83f, 65.10f, {0.00f, 0.50f, 0.35f, 0.10f});
    holes[4].hasPond   = true;
    holes[4].pondRadius = 2.0f;
    holes[4].obstacles.push_back({"Rock", glm::vec3(-1.6f, 0.25f, -1.0f), glm::vec3(1.0f), 0.0f});
    holes[4].obstacles.push_back({"Plank", glm::vec3(1.4f, 0.10f, 0.8f), glm::vec3(1.0f, 1.0f, 1.8f), 15.0f});

    // --- Hole 6
    setRectHole(holes[5], 9.33f, 6.02f, 25.23f, {0.00f, 0.45f, 0.45f, 0.00f});

    // --- Hole 7
    setRectHole(holes[6], 12.67f, 4.84f, 70.36f, {0.00f, 0.15f, 0.15f, 0.00f});
    holes[6].hasStream = true;
    holes[6].hasBridge = true;
    holes[6].streamPath = {{1.85f, -4.9f}, {1.85f, 4.9f}};

    // --- Hole 8
    setRectHole(holes[7], 12.14f, 4.49f, 42.52f, {0.00f, 0.35f, 0.60f, 0.20f});
    holes[7].obstacles.push_back({"Bunker", glm::vec3(-1.2f, 0.0f, -1.5f), glm::vec3(0.9f), 0.0f});
    holes[7].obstacles.push_back({"Bunker", glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.8f), 0.0f});
    holes[7].obstacles.push_back({"Bunker", glm::vec3(-0.1f, 0.0f, 1.6f), glm::vec3(0.85f), 0.0f});

    // --- Hole 9
    setRectHole(holes[8], 11.35f, 5.00f, 57.04f, {0.45f, 0.10f, 0.00f, 0.00f});
    holes[8].hasWindmill     = true;
    holes[8].windmillLocalPos = glm::vec3(0.4f, 0.0f, 1.8f);

    // --- Hole 10
    setRectHole(holes[9], 12.41f, 3.56f, 289.59f, {0.00f, 0.00f, 0.40f, 0.40f});

    // --- Hole 11
    setRectHole(holes[10], 9.88f, 5.70f, 176.87f, {0.00f, 0.15f, 0.45f, 0.10f});
    holes[10].obstacles.push_back({"Barrel", glm::vec3(0.0f, 0.35f, 0.6f), glm::vec3(1.0f), 0.0f});

    // --- Hole 12
    setRectHole(holes[11], 12.76f, 6.03f, 132.90f, {0.15f, 0.40f, 0.40f, 0.15f});
    holes[11].hasStream = true;
    holes[11].hasBridge = true;
    holes[11].streamPath = {{0.0f, -5.0f}, {0.0f, 5.0f}};
    holes[11].obstacles.push_back({"Rock", glm::vec3(-1.6f, 0.35f, -1.2f), glm::vec3(1.0f), 0.0f});
    holes[11].obstacles.push_back({"Rock", glm::vec3(0.2f, 0.35f, 0.3f), glm::vec3(0.9f), 0.0f});
    holes[11].obstacles.push_back({"Rock", glm::vec3(1.5f, 0.35f, 1.2f), glm::vec3(1.0f), 0.0f});

    // --- Hole 13
    setRectHole(holes[12], 11.77f, 4.16f, 144.09f, {0.35f, 0.25f, 0.20f, 0.40f});
    holes[12].obstacles.push_back({"Bunker", glm::vec3(-1.4f, 0.0f, 0.7f), glm::vec3(0.9f), 0.0f});
    holes[12].obstacles.push_back({"Rock", glm::vec3(1.3f, 0.35f, -0.2f), glm::vec3(0.85f), 20.0f});

    // --- Hole 14
    setRectHole(holes[13], 8.72f, 4.00f, 124.55f, {0.45f, 0.10f, 0.00f, 0.00f});

    // --- Hole 15
    setRectHole(holes[14], 18.24f, 4.42f, 231.82f, {0.00f, 0.15f, 0.35f, 0.25f});
    holes[14].hasPond    = true;
    holes[14].pondRadius  = 1.8f;
    holes[14].obstacles.push_back({"Bunker", glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec3(1.0f), 0.0f});

    // --- Hole 16
    setRectHole(holes[15], 15.69f, 4.15f, 130.68f, {0.30f, 0.05f, 0.00f, 0.25f});
    holes[15].hasStream = true;
    holes[15].streamPath = {{0.0f, -5.2f}, {0.0f, 5.2f}};
    holes[15].obstacles.push_back({"Bunker", glm::vec3(1.35f, 0.0f, -0.8f), glm::vec3(0.85f), 0.0f});

    // --- Hole 17
    setRectHole(holes[16], 16.92f, 5.00f, 110.63f, {0.05f, 0.08f, 0.10f, 0.06f});
    holes[16].hasStream = true;
    holes[16].streamPath = {{2.05f, -5.6f}, {2.05f, 5.6f}};
    holes[16].obstacles.push_back({"Rock", glm::vec3(-1.0f, 0.45f, 0.4f), glm::vec3(1.5f, 1.0f, 1.5f), 0.0f});
    holes[16].obstacles.push_back({"Plank", glm::vec3(1.3f, 0.10f, -0.6f), glm::vec3(1.4f, 1.0f, 0.8f), 5.0f});

    // --- Hole 18
    holes[17].par            = 4;
    holes[17].boundaryPoints = ellipseBounds(3.60f, 2.405f, 12);
    holes[17].elevations     = flatElevs(12);
    holes[17].rotation       = 54.64f;
    holes[17].hasTunnel      = true;
    {
        HoleConfig::ObstacleEntry e;
        e.type = "FlagPole"; e.localPos = glm::vec3(0.0f, 0.0f, -5.5f);
        e.scale = glm::vec3(1.0f); e.rotation = 0.0f;
        holes[17].decor.push_back(e);
    }

    return holes;
}
