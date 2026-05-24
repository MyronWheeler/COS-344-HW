#include "Water.h"
#include "Shader.h"
#include "TextureLoader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


static void buildWaterMesh(float w, float d, int nx, int nz,
                           GLuint &VAO, GLuint &VBO, GLuint &EBO,unsigned int &indexCount)
{
    struct WVert { float x,y,z, nx,ny,nz, u,v; };
    std::vector<WVert> verts;
    std::vector<unsigned int> idx;

    for (int j = 0; j <= nz; ++j) {
        for (int i = 0; i <= nx; ++i) {
            float fx = static_cast<float>(i) / nx;
            float fz = static_cast<float>(j) / nz;
            WVert v;
            v.x = (fx - 0.5f) * w; v.y = 0.0f; v.z = (fz - 0.5f) * d;
            v.nx = 0; v.ny = 1; v.nz = 0;
            v.u = fx * 4.0f; v.v = fz * 4.0f;   
            verts.push_back(v);
        }
    }
    for (int j = 0; j < nz; ++j) {
        for (int i = 0; i < nx; ++i) {
            unsigned int tl = static_cast<unsigned int>(j*(nx+1)+i);
            unsigned int tr = tl+1, bl = tl+(nx+1), br = bl+1;
            idx.push_back(tl); idx.push_back(bl); idx.push_back(tr);
            idx.push_back(tr); idx.push_back(bl); idx.push_back(br);
        }
    }
    indexCount = static_cast<unsigned int>(idx.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(verts.size() * sizeof(WVert)),verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_cast<GLsizeiptr>(idx.size() * sizeof(unsigned int)), idx.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WVert), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WVert), reinterpret_cast<void*>(12));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(WVert), reinterpret_cast<void*>(24));

    glBindVertexArray(0);
}

Water::Water(float width, float depth, glm::vec3 worldPos) {
    buildWaterMesh(width, depth, 20, 20, VAO, VBO, EBO, indexCount);
    normalMap   = TextureLoader::load("textures/water_normal.png");
    modelMatrix = glm::translate(glm::mat4(1.0f), worldPos);
}

void Water::draw(Shader &shader, float time, glm::mat4 view, glm::mat4 projection,glm::vec3 lightPos, glm::vec3 viewPos)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.use();
    shader.setMat4("model",modelMatrix);
    shader.setMat4("view",view);
    shader.setMat4("projection", projection);
    shader.setVec3("lightPos",lightPos);
    shader.setVec3("viewPos",viewPos);
    shader.setFloat("time",time);
    shader.setInt("normalMap",0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
