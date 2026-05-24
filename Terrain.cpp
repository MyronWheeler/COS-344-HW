#include "Terrain.h"
#include "Shader.h"
#include "TextureLoader.h"
#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

static glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::normalize(glm::cross(b - a, c - a));
}

static std::vector<glm::vec2> offsetPolygon(const std::vector<glm::vec2> &pts, float amount) {
    size_t n = pts.size();
    std::vector<glm::vec2> out(n);
    for (size_t i = 0; i < n; ++i) {
        glm::vec2 prev = pts[(i + n - 1) % n];
        glm::vec2 curr = pts[i];
        glm::vec2 next = pts[(i + 1) % n];

        glm::vec2 e1 = glm::normalize(curr - prev);
        glm::vec2 e2 = glm::normalize(next - curr);
        glm::vec2 n1( e1.y, -e1.x);
        glm::vec2 n2( e2.y, -e2.x);
        glm::vec2 bisector = glm::normalize(n1 + n2);

        float d = glm::dot(n1, bisector);
        float len = (d > 0.01f) ? amount / d : amount;
        out[i] = curr + bisector * len;
    }
    return out;
}

Mesh Terrain::buildFairway(const std::vector<glm::vec2> &boundary,
                            const std::vector<float>     &elevations)
{
    size_t n = boundary.size();
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    glm::vec2 centroid2(0.0f);
    float centroidY = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        centroid2 += boundary[i];
        centroidY += elevations[i];
    }
    centroid2 /= static_cast<float>(n);
    centroidY /= static_cast<float>(n);

    glm::vec3 centroid3(centroid2.x, centroidY, centroid2.y);

    Vertex cv;
    cv.position = centroid3;
    cv.normal   = glm::vec3(0, 1, 0);
    cv.texcoord = glm::vec2(0.5f, 0.5f);
    verts.push_back(cv);

    for (size_t i = 0; i < n; ++i) {
        Vertex v;
        v.position = glm::vec3(boundary[i].x, elevations[i], boundary[i].y);
        v.normal   = glm::vec3(0, 1, 0);
        v.texcoord = glm::vec2((boundary[i].x - centroid2.x) * 0.1f + 0.5f,
                               (boundary[i].y - centroid2.y) * 0.1f + 0.5f);
        verts.push_back(v);
    }

    for (size_t i = 0; i < n; ++i) {
        unsigned int a = 0;
        unsigned int b = static_cast<unsigned int>(1 + i);
        unsigned int c = static_cast<unsigned int>(1 + (i + 1) % n);
        glm::vec3 norm = computeNormal(verts[a].position, verts[b].position, verts[c].position);
        verts[a].normal = glm::normalize(verts[a].normal + norm);
        verts[b].normal = glm::normalize(verts[b].normal + norm);
        verts[c].normal = glm::normalize(verts[c].normal + norm);
        idx.push_back(a); idx.push_back(b); idx.push_back(c);
    }

    return Mesh(verts, idx);
}

Mesh Terrain::buildSurround(const std::vector<glm::vec2> &boundary, float minElevation) {
    const float OFFSET = 1.5f;
    std::vector<glm::vec2> outer = offsetPolygon(boundary, OFFSET);
    size_t n = boundary.size();

    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    for (size_t i = 0; i < n; ++i) {
        Vertex inner, outerV;
        inner.position  = glm::vec3(boundary[i].x, minElevation, boundary[i].y);
        inner.normal    = glm::vec3(0, 1, 0);
        inner.texcoord  = glm::vec2(static_cast<float>(i) / n, 0.0f);

        outerV.position = glm::vec3(outer[i].x, minElevation, outer[i].y);
        outerV.normal   = glm::vec3(0, 1, 0);
        outerV.texcoord = glm::vec2(static_cast<float>(i) / n, 1.0f);

        verts.push_back(inner);
        verts.push_back(outerV);
    }

    for (size_t i = 0; i < n; ++i) {
        unsigned int i0 = static_cast<unsigned int>(2 * i);
        unsigned int i1 = i0 + 1;
        unsigned int i2 = static_cast<unsigned int>(2 * ((i + 1) % n));
        unsigned int i3 = i2 + 1;
        idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
        idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
    }

    return Mesh(verts, idx);
}

// Manual min — replaces std::min_element (no <algorithm> needed)
static float vecMin(const std::vector<float> &v) {
    float m = v[0];
    for (size_t i = 1; i < v.size(); ++i)
        if (v[i] < m) m = v[i];
    return m;
}

Terrain::Terrain(const std::vector<glm::vec2> &boundary,
                 const std::vector<float>     &elevations)
    : fairway(buildFairway(boundary, elevations))
    , surround(buildSurround(boundary, vecMin(elevations)))
{}

void Terrain::draw(Shader &shader, glm::mat4 worldTransform) {
    shader.use();

    shader.setMat4("model", worldTransform);
    shader.setVec3("objectColor", glm::vec3(0.13f, 0.55f, 0.13f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/grass.png"));
    shader.setInt("objectTexture", 0);
    shader.setInt("useTexture", 1);
    fairway.draw();

    shader.setVec3("objectColor", glm::vec3(0.55f, 0.50f, 0.40f));
    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/gravel.png"));
    surround.draw();
    shader.setInt("useTexture", 0);
}
