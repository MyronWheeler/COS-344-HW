#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"

class Shader;
class Terrain {
public:
    Terrain(const std::vector<glm::vec2> &boundary,const std::vector<float>     &elevations);
    void draw(Shader &shader, glm::mat4 worldTransform);

private:
    Mesh fairway;   
    Mesh surround;  
    static Mesh buildFairway(const std::vector<glm::vec2> &boundary, const std::vector<float>     &elevations);
    static Mesh buildSurround(const std::vector<glm::vec2> &boundary, float minElevation);
};
