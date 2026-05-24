#include "Terrain.h"
#include "Shader.h"
#include "TextureLoader.h"
#include <GL/glew.h>
#include <cmath>

static glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::normalize(glm::cross(c - a, b - a));
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

Mesh Terrain::buildFairway(const std::vector<glm::vec2> &boundary,const std::vector<float>     &elevations)
{
    size_t n = boundary.size();
    std::vector<Vertex> verts;
    std::vector<unsigned int> idx;

    
    glm::vec2 centroid2(0.0f);
    for (size_t i = 0; i < n; ++i)
        centroid2 += boundary[i];
    centroid2 /= static_cast<float>(n);

    
    
    float humpHeight = 0.0f;
    for (float e : elevations) humpHeight += e;
    humpHeight /= static_cast<float>(elevations.size());
    humpHeight *= 0.6f;

    
    
    float maxRadius = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float d = glm::length(boundary[i] - centroid2);
        if (d > maxRadius) maxRadius = d;
    }

    
    Vertex cv;
    cv.position = glm::vec3(centroid2.x, humpHeight, centroid2.y);
    cv.normal   = glm::vec3(0, 1, 0);
    cv.texcoord = glm::vec2(0.5f, 0.5f);
    verts.push_back(cv);

    
    for (size_t i = 0; i < n; ++i) {
        Vertex v;
        v.position = glm::vec3(boundary[i].x, 0.0f, boundary[i].y);
        v.normal   = glm::vec3(0, 1, 0);
        v.texcoord = glm::vec2((boundary[i].x - centroid2.x) * 0.1f + 0.5f, (boundary[i].y - centroid2.y) * 0.1f + 0.5f);
        verts.push_back(v);
    }

    
    for (size_t i = 0; i < n; ++i) {
        unsigned int a = 0;
        unsigned int b = static_cast<unsigned int>(1 + i);
        unsigned int c = static_cast<unsigned int>(1 + (i + 1) % n);
        glm::vec3 norm = computeNormal(verts[a].position, verts[c].position, verts[b].position);
        verts[a].normal = glm::normalize(verts[a].normal + norm);
        verts[b].normal = glm::normalize(verts[b].normal + norm);
        verts[c].normal = glm::normalize(verts[c].normal + norm);
        idx.push_back(a); idx.push_back(c); idx.push_back(b);
    }

    for (auto &v : verts)
        if (v.normal.y < 0.0f) v.normal *= -1.0f;

    return Mesh(verts, idx);
}

Mesh Terrain::buildSurround(const std::vector<glm::vec2> &boundary, float minElevation) {
    const float OFFSET = 3.0f;
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

        verts.push_back(inner);
        verts.push_back(outerV);
    }

    
    for (size_t i = 0; i < n; ++i) {
        unsigned int i0 = static_cast<unsigned int>(2 * i);
        unsigned int i1 = i0 + 1;
        unsigned int i2 = static_cast<unsigned int>(2 * ((i + 1) % n));
        unsigned int i3 = i2 + 1;
        idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
        idx.push_back(i2); idx.push_back(i3); idx.push_back(i1);
    }

    return Mesh(verts, idx);
}

Terrain::Terrain(const std::vector<glm::vec2> &boundary,
                 const std::vector<float>     &elevations)
    : fairway(buildFairway(boundary, elevations))
    , surround(buildSurround(boundary, 0.15f))
{}

void Terrain::draw(Shader &shader, glm::mat4 worldTransform) {
    shader.use();
    shader.setMat4("model", worldTransform);

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/grass.png"));
    shader.setInt("objectTexture", 0);
    shader.setInt("useTexture", 1);
    fairway.draw();

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureLoader::load("textures/gravel.png"));
    shader.setInt("objectTexture", 0);
    shader.setInt("useTexture", 1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);
    surround.draw();
    glDisable(GL_POLYGON_OFFSET_FILL);

    shader.setInt("useTexture", 0);
}
