#include "Terrain.h"
#include "Shader.h"
#include "TextureLoader.h"
#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::normalize(glm::cross(b - a, c - a));
}

// Offset a 2-D polygon outward by 'amount' units (simple per-vertex normal method)
static std::vector<glm::vec2> offsetPolygon(const std::vector<glm::vec2> &pts, float amount) {
    size_t n = pts.size();
    std::vector<glm::vec2> out(n);
    for (size_t i = 0; i < n; ++i) {
        glm::vec2 prev = pts[(i + n - 1) % n];
        glm::vec2 curr = pts[i];
        glm::vec2 next = pts[(i + 1) % n];

        glm::vec2 e1 = glm::normalize(curr - prev);
        glm::vec2 e2 = glm::normalize(next - curr);

        // Outward normals (right-hand side of each edge for CCW polygon)
        glm::vec2 n1( e1.y, -e1.x);
        glm::vec2 n2( e2.y, -e2.x);
        glm::vec2 bisector = glm::normalize(n1 + n2);

        float dot = glm::dot(n1, bisector);
        float len = (dot > 0.01f) ? amount / dot : amount;
        out[i] = curr + bisector * len;
    }
    return out;
}

// ---------------------------------------------------------------------------
// Terrain::buildFairway
// Fan triangulation from centroid
// ---------------------------------------------------------------------------
Mesh Terrain::buildFairway(const std::vector<glm::vec2> &boundary,
                            const std::vector<float>     &elevations)
{
    size_t n = boundary.size();
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    // Centroid (average position, average elevation)
    glm::vec2 centroid2(0.0f);
    float centroidY = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        centroid2 += boundary[i];
        centroidY += elevations[i];
    }
    centroid2 /= static_cast<float>(n);
    centroidY /= static_cast<float>(n);

    glm::vec3 centroid3(centroid2.x, centroidY, centroid2.y);

    // Centre vertex
    Vertex cv;
    cv.position = centroid3;
    cv.normal   = glm::vec3(0.0f, 1.0f, 0.0f);
    cv.texcoord = glm::vec2(0.5f, 0.5f);
    verts.push_back(cv);

    // Boundary vertices
    for (size_t i = 0; i < n; ++i) {
        Vertex v;
        v.position = glm::vec3(boundary[i].x, elevations[i], boundary[i].y);
        v.normal   = glm::vec3(0.0f, 1.0f, 0.0f);  // refined below per tri
        float u = (boundary[i].x - centroid2.x) * 0.1f + 0.5f;
        float vv = (boundary[i].y - centroid2.y) * 0.1f + 0.5f;
        v.texcoord = glm::vec2(u, vv);
        verts.push_back(v);
    }

    // Fan triangles: centre=0, ring starts at 1
    for (size_t i = 0; i < n; ++i) {
        unsigned int a = 0;
        unsigned int b = static_cast<unsigned int>(1 + i);
        unsigned int c = static_cast<unsigned int>(1 + (i + 1) % n);

        // Per-face normal
        glm::vec3 norm = computeNormal(verts[a].position, verts[b].position, verts[c].position);
        verts[a].normal = glm::normalize(verts[a].normal + norm);
        verts[b].normal = glm::normalize(verts[b].normal + norm);
        verts[c].normal = glm::normalize(verts[c].normal + norm);

        idx.push_back(a);
        idx.push_back(b);
        idx.push_back(c);
    }

    return Mesh(verts, idx);
}

// ---------------------------------------------------------------------------
// Terrain::buildSurround
// Quad strip between original boundary and outward-offset boundary
// ---------------------------------------------------------------------------
Mesh Terrain::buildSurround(const std::vector<glm::vec2> &boundary, float minElevation) {
    const float OFFSET = 1.5f;
    std::vector<glm::vec2> outer = offsetPolygon(boundary, OFFSET);
    size_t n = boundary.size();

    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    for (size_t i = 0; i < n; ++i) {
        Vertex inner, outerV;
        inner.position  = glm::vec3(boundary[i].x, minElevation, boundary[i].y);
        inner.normal    = glm::vec3(0.0f, 1.0f, 0.0f);
        inner.texcoord  = glm::vec2(static_cast<float>(i) / n, 0.0f);

        outerV.position = glm::vec3(outer[i].x, minElevation, outer[i].y);
        outerV.normal   = glm::vec3(0.0f, 1.0f, 0.0f);
        outerV.texcoord = glm::vec2(static_cast<float>(i) / n, 1.0f);

        verts.push_back(inner);   // 2i
        verts.push_back(outerV);  // 2i+1
    }

    for (size_t i = 0; i < n; ++i) {
        unsigned int i0 = static_cast<unsigned int>(2 * i);
        unsigned int i1 = static_cast<unsigned int>(2 * i + 1);
        unsigned int i2 = static_cast<unsigned int>(2 * ((i + 1) % n));
        unsigned int i3 = static_cast<unsigned int>(2 * ((i + 1) % n) + 1);

        idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
        idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
    }

    return Mesh(verts, idx);
}

// ---------------------------------------------------------------------------
// Terrain ctor / draw
// ---------------------------------------------------------------------------
Terrain::Terrain(const std::vector<glm::vec2> &boundary,
                 const std::vector<float>     &elevations)
    : fairway(buildFairway(boundary, elevations))
    , surround(buildSurround(boundary,
                             *std::min_element(elevations.begin(), elevations.end())))
{}

void Terrain::draw(Shader &shader, glm::mat4 worldTransform) {
    shader.use();

    // Fairway — green turf
    shader.setMat4("model", worldTransform);
    shader.setVec3("objectColor", glm::vec3(0.13f, 0.55f, 0.13f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/grass.png"));
    shader.setInt("diffuseTex", 0);
    fairway.draw();

    // Surround — gravel / rough
    shader.setVec3("objectColor", glm::vec3(0.55f, 0.50f, 0.40f));
    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/gravel.png"));
    surround.draw();
}
