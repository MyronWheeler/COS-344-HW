#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const std::string &vertPath, const std::string &fragPath);
    ~Shader();

    void use() const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;

private:
    static std::string readFile(const std::string &path);
    static unsigned int compileShader(unsigned int type, const std::string &source, const std::string &label);
};
