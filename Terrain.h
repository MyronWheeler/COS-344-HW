#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"

class Shader;

class Terrain {
public:
    // boundary: 2-D polygon in local XZ space
    // elevations: Y value for each boundary vertex (same length as boundary)
    Terrain(const std::vector<glm::vec2> &boundary,
            const std::vector<float>     &elevations);

    void draw(Shader &shader, glm::mat4 worldTransform);

private:
    Mesh fairway;   // tessellated polygon at given elevations
    Mesh surround;  // gravel skirt — boundary offset outward, flat at min elevation

    static Mesh buildFairway(const std::vector<glm::vec2> &boundary,
                              const std::vector<float>     &elevations);

    static Mesh buildSurround(const std::vector<glm::vec2> &boundary,
                               float minElevation);
};
