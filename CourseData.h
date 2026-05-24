#pragma once

#include "holes/Hole01.h"
#include "holes/Hole02.h"
#include "holes/Hole03.h"
#include "holes/Hole04.h"
#include "holes/Hole05.h"
#include "holes/Hole06.h"
#include "holes/Hole07.h"
#include "holes/Hole08.h"
#include "holes/Hole09.h"
#include "holes/Hole10.h"
#include "holes/Hole11.h"
#include "holes/Hole12.h"
#include "holes/Hole13.h"
#include "holes/Hole14.h"
#include "holes/Hole15.h"
#include "holes/Hole16.h"
#include "holes/Hole17.h"
#include "holes/Hole18.h"

#include <vector>

static const float HOLE_X[18] = {
    -42.0f, -26.0f,  -8.0f,   8.0f,  26.0f,  42.0f,
     42.0f,  26.0f,   8.0f,  -8.0f, -26.0f, -42.0f,
    -42.0f, -26.0f,  -8.0f,   8.0f,  26.0f,  42.0f,
};
static const float HOLE_Z[18] = {
    -28.0f, -30.0f, -28.0f, -30.0f, -28.0f, -30.0f,
      0.0f,   2.0f,   0.0f,   2.0f,   0.0f,   2.0f,
     28.0f,  30.0f,  28.0f,  30.0f,  28.0f,  30.0f,
};

inline std::vector<HoleConfig> buildCourseData() {
    std::vector<HoleConfig> holes = {
        buildHole01(), buildHole02(), buildHole03(),
        buildHole04(), buildHole05(), buildHole06(),
        buildHole07(), buildHole08(), buildHole09(),
        buildHole10(), buildHole11(), buildHole12(),
        buildHole13(), buildHole14(), buildHole15(),
        buildHole16(), buildHole17(), buildHole18(),
    };
    for (int i = 0; i < 18; ++i)
        holes[i].position = glm::vec3(HOLE_X[i], holes[i].position.y, HOLE_Z[i]);
    return holes;
}
