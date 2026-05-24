#pragma once

#include "HoleConfig.h"
#include <cmath>
#include <vector>

// ---------------------------------------------------------------------------
// Research-based layout data for the Vines mini golf course.
// ---------------------------------------------------------------------------

static inline std::vector<glm::vec2> roundedRectBounds(float length, float width, float radius = 1.0f, int cornerSegments = 6) {
    std::vector<glm::vec2> points;
    float hw = width * 0.5f;
    float hl = length * 0.5f;
    float iw = hw - radius;
    float il = hl - radius;
    if (iw < 0) iw = 0;
    if (il < 0) il = 0;
    const float PI = 3.14159265f;
    
    for (int i = 0; i <= cornerSegments; ++i) { float t = (PI / 2.0f) * (static_cast<float>(i) / cornerSegments); points.push_back({iw + radius * std::cos(t), il + radius * std::sin(t)}); }
    for (int i = 0; i <= cornerSegments; ++i) { float t = (PI / 2.0f) + (PI / 2.0f) * (static_cast<float>(i) / cornerSegments); points.push_back({-iw + radius * std::cos(t), il + radius * std::sin(t)}); }
    for (int i = 0; i <= cornerSegments; ++i) { float t = PI + (PI / 2.0f) * (static_cast<float>(i) / cornerSegments); points.push_back({-iw + radius * std::cos(t), -il + radius * std::sin(t)}); }
    for (int i = 0; i <= cornerSegments; ++i) { float t = (3.0f * PI / 2.0f) + (PI / 2.0f) * (static_cast<float>(i) / cornerSegments); points.push_back({iw + radius * std::cos(t), -il + radius * std::sin(t)}); }
    
    return points;
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

static inline std::vector<glm::vec2> lShapeBounds(float mhw, float mhl, float ahw, float ahl, float ox) {
    float x0 = -mhw,  x1 = mhw;
    float z0 = -mhl,  z1 =  mhl;
    float ax0 = ox - ahw, ax1 = ox + ahw;
    float az1 = z1 + ahl;
    return {
        { x0,  z0}, { x1,  z0}, { x1,  z1}, {ax1,  z1},
        {ax1, az1}, {ax0, az1}, {ax0,  z1}, { x0,  z1},
    };
}

// Added new boundary generator specifically for Hole 14 based on the CSV!
static inline std::vector<glm::vec2> semiCircleBounds(float radius, int segments = 16) {
    std::vector<glm::vec2> points;
    const float PI = 3.14159265f;
    for (int i = 0; i <= segments; ++i) {
        float t = PI * static_cast<float>(i) / static_cast<float>(segments);
        points.push_back({radius * std::cos(t), radius * std::sin(t)});
    }
    return points;
}

static inline std::vector<float> flatElevs(int n, float y = 0.0f) { return std::vector<float>(static_cast<size_t>(n), y); }

static inline void setRectHole(HoleConfig &hole, float length, float width, float heading, std::vector<float> elevations) {
    hole.boundaryPoints = roundedRectBounds(length, width, 1.2f, 8); 
    if (elevations.size() < hole.boundaryPoints.size()) {
        std::vector<float> paddedElevs(hole.boundaryPoints.size(), elevations.empty() ? 0.0f : elevations[0]);
        hole.elevations = paddedElevs;
    } else { hole.elevations = std::move(elevations); }
    hole.rotation = heading;
}

static inline void setLShapeHole(HoleConfig &hole, float mhw, float mhl, float ahw, float ahl, float ox, float heading, std::vector<float> elevations) {
    hole.boundaryPoints = lShapeBounds(mhw, mhl, ahw, ahl, ox);
    if (elevations.size() < hole.boundaryPoints.size()) {
        std::vector<float> paddedElevs(hole.boundaryPoints.size(), elevations.empty() ? 0.0f : elevations[0]);
        hole.elevations = paddedElevs;
    } else { hole.elevations = std::move(elevations); }
    hole.rotation = heading;
}

static inline void setSemiCircleHole(HoleConfig &hole, float radius, float heading, std::vector<float> elevations) {
    hole.boundaryPoints = semiCircleBounds(radius);
    if (elevations.size() < hole.boundaryPoints.size()) {
        std::vector<float> paddedElevs(hole.boundaryPoints.size(), elevations.empty() ? 0.0f : elevations[0]);
        hole.elevations = paddedElevs;
    } else { hole.elevations = std::move(elevations); }
    hole.rotation = heading;
}

inline std::vector<HoleConfig> buildCourseData() {
    std::vector<HoleConfig> holes;
    holes.reserve(18);

    // Sprawled out coordinates taking advantage of the 160x120 ground plane!
    static const float HOLE_X[18] = {
        -60.0f, -40.0f, -15.0f,   5.0f,  30.0f,  55.0f,
         60.0f,  45.0f,  20.0f,   0.0f, -25.0f, -45.0f,
        -65.0f, -35.0f, -10.0f,  15.0f,  40.0f,  65.0f
    };
    static const float HOLE_Z[18] = {
        -35.0f, -45.0f, -30.0f, -40.0f, -25.0f, -40.0f,
         -5.0f,  10.0f,   0.0f,  15.0f,   5.0f,  20.0f,
         30.0f,  45.0f,  35.0f,  50.0f,  30.0f,  45.0f
    };

    int parCycle[4] = {3, 3, 4, 3};

    for (int i = 0; i < 18; ++i) {
        HoleConfig h;
        h.holeNumber = i + 1;
        h.par        = parCycle[i % 4];
        h.position   = glm::vec3(HOLE_X[i], 0.5f, HOLE_Z[i]);
        h.rotation   = (i * 35.0f) + 15.0f; 
        h.boundaryPoints = roundedRectBounds(13.0f, 3.0f);
        h.elevations     = flatElevs(h.boundaryPoints.size());
        holes.push_back(h);
    }

    // ---- Per-hole overrides driven by the CSV spreadsheet -------------------
    
    // Hole 1: Elevated at start and end
    setRectHole(holes[0], 15.69f, 4.55f, 115.00f, {0.55f});
    
    // Hole 2: "curves to the left" -> Make it an L-Shape
    setLShapeHole(holes[1], 1.8f, 6.0f, 1.8f, 4.0f, -1.0f, 224.04f, {0.45f});
    
    // Hole 3: Elevated only in the middle
    setRectHole(holes[2], 9.76f, 3.93f, 210.80f, {0.20f});
    
    // Hole 4: Elevated near start, dips at end
    setRectHole(holes[3], 15.00f, 3.35f, 109.35f, {0.40f});
    
    // Hole 5: "very simple and straight", rocks and wooden planks
    setRectHole(holes[4], 15.51f, 5.83f, 65.10f, {0.00f});
    holes[4].hasPond   = true;
    holes[4].pondRadius = 2.0f;
    // Surface = 0.00 + 0.10 = 0.10
    holes[4].obstacles.push_back({"Rock", glm::vec3(-1.6f, 0.25f, -1.0f), glm::vec3(1.0f), 0.0f});
    holes[4].obstacles.push_back({"Plank", glm::vec3(1.4f, 0.15f, 0.8f), glm::vec3(1.0f, 1.0f, 1.8f), 15.0f});

    // Hole 6: 2 speed humps in the middle
    setRectHole(holes[5], 9.33f, 6.02f, 25.23f, {0.45f});
    
    // Hole 7
    setRectHole(holes[6], 12.67f, 4.84f, 70.36f, {0.15f});

    // Hole 8: 3 speed humps, 3 bunkers
    setRectHole(holes[7], 12.14f, 4.49f, 42.52f, {0.35f});
    // Surface = 0.35 + 0.10 = 0.45
    holes[7].obstacles.push_back({"Bunker", glm::vec3(-1.2f, 0.46f, -1.5f), glm::vec3(0.9f), 0.0f});
    holes[7].obstacles.push_back({"Bunker", glm::vec3(0.9f, 0.46f, 0.0f), glm::vec3(0.8f), 0.0f});
    holes[7].obstacles.push_back({"Bunker", glm::vec3(-0.1f, 0.46f, 1.6f), glm::vec3(0.85f), 0.0f});

    // Hole 9: Windmill, "weird shape" -> Make it an L-Shape!
    setLShapeHole(holes[8], 2.5f, 5.6f, 2.0f, 4.0f, 1.5f, 57.04f, {0.45f});
    holes[8].hasWindmill     = true;
    // Surface = 0.45 + 0.10 = 0.55
    holes[8].windmillLocalPos = glm::vec3(0.4f, 0.55f, 1.8f);

    // Hole 10: Elevated at the end
    setRectHole(holes[9], 12.41f, 3.56f, 289.59f, {0.00f});
    
    // Hole 11: Cylindrical obstacle
    setRectHole(holes[10], 9.88f, 5.70f, 176.87f, {0.15f});
    // Surface = 0.15 + 0.10 = 0.25
    holes[10].obstacles.push_back({"Barrel", glm::vec3(0.0f, 0.60f, 0.6f), glm::vec3(1.0f), 0.0f});

    // Hole 12: 3 rock obstacles
    setRectHole(holes[11], 12.76f, 6.03f, 132.90f, {0.15f});
    // Surface = 0.15 + 0.10 = 0.25
    holes[11].obstacles.push_back({"Rock", glm::vec3(-1.6f, 0.40f, -1.2f), glm::vec3(1.0f), 0.0f});
    holes[11].obstacles.push_back({"Rock", glm::vec3(0.2f, 0.40f, 0.3f), glm::vec3(0.9f), 0.0f});
    holes[11].obstacles.push_back({"Rock", glm::vec3(1.5f, 0.40f, 1.2f), glm::vec3(1.0f), 0.0f});

    // Hole 13: Bunker right, Rock left
    setLShapeHole(holes[12], 2.0f, 6.0f, 2.0f, 5.0f, -1.0f, 144.09f, {0.35f});
    // Surface = 0.35 + 0.10 = 0.45
    holes[12].obstacles.push_back({"Bunker", glm::vec3(1.4f, 0.46f, 0.7f), glm::vec3(0.9f), 0.0f});
    holes[12].obstacles.push_back({"Rock", glm::vec3(-1.3f, 0.60f, -0.2f), glm::vec3(0.85f), 20.0f});

    // Hole 14: "This hole is literally a semi circle" -> Use new helper!
    // 8.72m long, 4m wide -> roughly a radius of 4.36m
    setSemiCircleHole(holes[13], 4.36f, 124.55f, {0.45f});
    
    // Hole 15: Bunker to its left
    setRectHole(holes[14], 18.24f, 4.42f, 231.82f, {0.15f});
    holes[14].hasPond    = true;
    holes[14].pondRadius  = 1.8f;
    // Surface = 0.15 + 0.10 = 0.25
    holes[14].obstacles.push_back({"Bunker", glm::vec3(-1.5f, 0.26f, 0.0f), glm::vec3(1.0f), 0.0f});

    // Hole 16: Bunker to its right
    setRectHole(holes[15], 15.69f, 4.15f, 130.68f, {0.30f});
    // Surface = 0.30 + 0.10 = 0.40
    holes[15].obstacles.push_back({"Bunker", glm::vec3(1.35f, 0.41f, -0.8f), glm::vec3(0.85f), 0.0f});

    // Hole 17: Rock to left, Wooden plank to right
    setRectHole(holes[16], 16.92f, 5.00f, 110.63f, {0.05f});
    // Surface = 0.05 + 0.10 = 0.15
    holes[16].obstacles.push_back({"Rock", glm::vec3(-1.0f, 0.30f, 0.4f), glm::vec3(1.5f, 1.0f, 1.5f), 0.0f});
    holes[16].obstacles.push_back({"Plank", glm::vec3(1.3f, 0.20f, -0.6f), glm::vec3(1.4f, 1.0f, 0.8f), 5.0f});

    // --- Hole 18
    holes[17].par            = 4;
    holes[17].boundaryPoints = ellipseBounds(3.60f, 2.405f, 24); 
    holes[17].elevations     = flatElevs(24);
    holes[17].rotation       = 54.64f;
    holes[17].hasTunnel      = true;
    {
        HoleConfig::ObstacleEntry e;
        // Surface = 0.00 + 0.10 = 0.10
        e.type = "FlagPole"; e.localPos = glm::vec3(0.0f, 0.10f, -5.5f);
        e.scale = glm::vec3(1.0f); e.rotation = 0.0f;
        holes[17].decor.push_back(e);
    }

    return holes;
}