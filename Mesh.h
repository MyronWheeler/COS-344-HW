#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class Mesh {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
        ~Mesh();

        void draw() const;

        static Mesh createPlane(float width, float depth, int subdivisionsX, int subdivisionsZ);
        static Mesh createBox(float w, float h, float d);
        static Mesh createCylinder(float radius, float height, int segments);
        static Mesh createSphere(float radius, int segments);
        static Mesh createTriangularPrism(float w, float h, float d);

    private:
        unsigned int VAO, VBO, EBO;
        unsigned int indexCount;
};
