#pragma once

#include "Hole01.h"
#include "Hole02.h"
#include "Hole03.h"
#include "Hole04.h"
#include "Hole05.h"
#include "Hole06.h"
#include "Hole07.h"
#include "Hole08.h"
#include "Hole09.h"
#include "Hole10.h"
#include "Hole11.h"
#include "Hole12.h"
#include "Hole13.h"
#include "Hole14.h"
#include "Hole15.h"
#include "Hole16.h"
#include "Hole17.h"
#include "Hole18.h"

#include <vector>

inline std::vector<HoleConfig> buildCourseData() {
    return {
        buildHole01(), buildHole02(), buildHole03(),
        buildHole04(), buildHole05(), buildHole06(),
        buildHole07(), buildHole08(), buildHole09(),
        buildHole10(), buildHole11(), buildHole12(),
        buildHole13(), buildHole14(), buildHole15(),
        buildHole16(), buildHole17(), buildHole18(),
    };
}
