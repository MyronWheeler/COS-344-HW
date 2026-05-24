#include "Mesh.h"

#include <GL/glew.h>
#include <cmath>

static const float PI = 3.14159265358979323846f;

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
    : indexCount(static_cast<unsigned int>(indices.size()))
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    // texcoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, texcoord)));

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh Mesh::createPlane(float width, float depth, int subdivisionsX, int subdivisionsZ) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    int nx = subdivisionsX + 1;
    int nz = subdivisionsZ + 1;

    for (int z = 0; z < nz; ++z) {
        for (int x = 0; x < nx; ++x) {
            float fx = static_cast<float>(x) / subdivisionsX;
            float fz = static_cast<float>(z) / subdivisionsZ;
            Vertex v;
            v.position  = glm::vec3((fx - 0.5f) * width, 0.0f, (fz - 0.5f) * depth);
            v.normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            v.texcoord  = glm::vec2(fx, fz);
            verts.push_back(v);
        }
    }

    for (int z = 0; z < subdivisionsZ; ++z) {
        for (int x = 0; x < subdivisionsX; ++x) {
            unsigned int tl = static_cast<unsigned int>(z * nx + x);
            unsigned int tr = tl + 1;
            unsigned int bl = tl + static_cast<unsigned int>(nx);
            unsigned int br = bl + 1;
            idx.push_back(tl); idx.push_back(bl); idx.push_back(tr);
            idx.push_back(tr); idx.push_back(bl); idx.push_back(br);
        }
    }

    return Mesh(verts, idx);
}

Mesh Mesh::createBox(float w, float h, float d) {
    float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;

    // 6 faces × 4 vertices
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    auto addFace = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n) {
        unsigned int base = static_cast<unsigned int>(verts.size());
        verts.push_back({p0, n, glm::vec2(0, 0)});
        verts.push_back({p1, n, glm::vec2(1, 0)});
        verts.push_back({p2, n, glm::vec2(1, 1)});
        verts.push_back({p3, n, glm::vec2(0, 1)});
        idx.push_back(base);     idx.push_back(base + 1); idx.push_back(base + 2);
        idx.push_back(base);     idx.push_back(base + 2); idx.push_back(base + 3);
    };

    // +Y top — CCW from above so the face is not culled when viewed from above
    addFace({-hw, hh, -hd}, {-hw, hh,  hd}, { hw, hh,  hd}, { hw, hh, -hd}, { 0, 1, 0});
    // -Y bottom
    addFace({-hw,-hh,  hd}, { hw,-hh,  hd}, { hw,-hh, -hd}, {-hw,-hh, -hd}, { 0,-1, 0});
    // +Z front
    addFace({-hw,-hh,  hd}, { hw,-hh,  hd}, { hw, hh,  hd}, {-hw, hh,  hd}, { 0, 0, 1});
    // -Z back
    addFace({ hw,-hh, -hd}, {-hw,-hh, -hd}, {-hw, hh, -hd}, { hw, hh, -hd}, { 0, 0,-1});
    // +X right
    addFace({ hw,-hh,  hd}, { hw,-hh, -hd}, { hw, hh, -hd}, { hw, hh,  hd}, { 1, 0, 0});
    // -X left
    addFace({-hw,-hh, -hd}, {-hw,-hh,  hd}, {-hw, hh,  hd}, {-hw, hh, -hd}, {-1, 0, 0});

    return Mesh(verts, idx);
}

Mesh Mesh::createCylinder(float radius, float height, int segments) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    float hh = height * 0.5f;

    // Side vertices: two rings
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * static_cast<float>(i) / segments;
        float cx = cosf(angle), cz = sinf(angle);
        glm::vec3 n(cx, 0.0f, cz);
        float u = static_cast<float>(i) / segments;

        Vertex bot, top;
        bot.position = glm::vec3(cx * radius, -hh, cz * radius);
        bot.normal   = n;
        bot.texcoord = glm::vec2(u, 0.0f);

        top.position = glm::vec3(cx * radius,  hh, cz * radius);
        top.normal   = n;
        top.texcoord = glm::vec2(u, 1.0f);

        verts.push_back(bot);
        verts.push_back(top);
    }

    // Side indices
    for (int i = 0; i < segments; ++i) {
        unsigned int b0 = static_cast<unsigned int>(i * 2);
        unsigned int t0 = b0 + 1;
        unsigned int b1 = b0 + 2;
        unsigned int t1 = b0 + 3;
        idx.push_back(b0); idx.push_back(t0); idx.push_back(b1);
        idx.push_back(t0); idx.push_back(t1); idx.push_back(b1);
    }

    // Cap vertices
    auto addCap = [&](float y, glm::vec3 n) {
        unsigned int centerIdx = static_cast<unsigned int>(verts.size());
        verts.push_back({{0.0f, y, 0.0f}, n, {0.5f, 0.5f}});

        for (int i = 0; i <= segments; ++i) {
            float angle = 2.0f * PI * static_cast<float>(i) / segments;
            float cx = cosf(angle), cz = sinf(angle);
            verts.push_back({{cx * radius, y, cz * radius}, n,
                              {cx * 0.5f + 0.5f, cz * 0.5f + 0.5f}});
        }

        for (int i = 0; i < segments; ++i) {
            unsigned int a = centerIdx + 1 + static_cast<unsigned int>(i);
            unsigned int b = a + 1;
            if (n.y > 0) {
                idx.push_back(centerIdx); idx.push_back(a); idx.push_back(b);
            } else {
                idx.push_back(centerIdx); idx.push_back(b); idx.push_back(a);
            }
        }
    };

    addCap(-hh, {0.0f, -1.0f, 0.0f});
    addCap( hh, {0.0f,  1.0f, 0.0f});

    return Mesh(verts, idx);
}

Mesh Mesh::createSphere(float radius, int segments) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    int stacks = segments / 2;
    int slices = segments;

    for (int i = 0; i <= stacks; ++i) {
        float phi = PI * static_cast<float>(i) / stacks;
        float sp  = sinf(phi), cp = cosf(phi);
        float v   = static_cast<float>(i) / stacks;

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * PI * static_cast<float>(j) / slices;
            float st = sinf(theta), ct = cosf(theta);
            float u = static_cast<float>(j) / slices;

            glm::vec3 n(sp * ct, cp, sp * st);
            Vertex vert;
            vert.position = n * radius;
            vert.normal   = n;
            vert.texcoord = glm::vec2(u, v);
            verts.push_back(vert);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned int tl = static_cast<unsigned int>(i * (slices + 1) + j);
            unsigned int tr = tl + 1;
            unsigned int bl = tl + static_cast<unsigned int>(slices + 1);
            unsigned int br = bl + 1;
            idx.push_back(tl); idx.push_back(tr); idx.push_back(bl);
            idx.push_back(tr); idx.push_back(br); idx.push_back(bl);
        }
    }

    return Mesh(verts, idx);
}

Mesh Mesh::createTriangularPrism(float w, float h, float d) {
    float hw = w * 0.5f;
    float hd = d * 0.5f;

    glm::vec3 BFL(-hw, 0.0f, -hd);
    glm::vec3 BFR( hw, 0.0f, -hd);
    glm::vec3 BBR( hw, 0.0f,  hd);
    glm::vec3 BBL(-hw, 0.0f,  hd);
    glm::vec3 TF ( 0.0f,  h, -hd);
    glm::vec3 TB ( 0.0f,  h,  hd);

    glm::vec3 nL = glm::normalize(glm::vec3(-h, hw, 0.0f));
    glm::vec3 nR = glm::normalize(glm::vec3( h, hw, 0.0f));

    std::vector<Vertex> verts;
    std::vector<unsigned int> idx2;

    auto addQuad = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n) {
        unsigned int base = static_cast<unsigned int>(verts.size());
        verts.push_back({p0, n, {0.0f, 0.0f}});
        verts.push_back({p1, n, {1.0f, 0.0f}});
        verts.push_back({p2, n, {1.0f, 1.0f}});
        verts.push_back({p3, n, {0.0f, 1.0f}});
        idx2.push_back(base);     idx2.push_back(base + 1); idx2.push_back(base + 2);
        idx2.push_back(base);     idx2.push_back(base + 2); idx2.push_back(base + 3);
    };
    auto addTri = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 n) {
        unsigned int base = static_cast<unsigned int>(verts.size());
        verts.push_back({p0, n, {0.5f, 0.0f}});
        verts.push_back({p1, n, {1.0f, 1.0f}});
        verts.push_back({p2, n, {0.0f, 1.0f}});
        idx2.push_back(base); idx2.push_back(base + 1); idx2.push_back(base + 2);
    };

    addQuad(BFL, BBL, TB,  TF,  nL);
    addQuad(BFR, TF,  TB,  BBR, nR);
    addTri (BFL, TF,  BFR, glm::vec3( 0.0f,  0.0f, -1.0f));
    addTri (BBR, TB,  BBL, glm::vec3( 0.0f,  0.0f,  1.0f));
    addQuad(BFL, BFR, BBR, BBL, glm::vec3( 0.0f, -1.0f,  0.0f));

    return Mesh(verts, idx2);
}
