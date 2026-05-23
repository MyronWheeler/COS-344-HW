#include "Shader.h"

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string &vertPath, const std::string &fragPath) {
    std::string vertSrc = readFile(vertPath);
    std::string fragSrc = readFile(fragPath);

    unsigned int vert = compileShader(GL_VERTEX_SHADER,   vertSrc, vertPath);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc, fragPath);

    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(ID, sizeof(log), nullptr, log);
        std::cerr << "[Shader] Link error:\n" << log << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

std::string Shader::readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Shader] Cannot open: " << path << std::endl;
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string &source, const std::string &label) {
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "[Shader] Compile error in " << label << ":\n" << log << std::endl;
    }
    return shader;
}
