#include "ComputeShader.hpp"

#include <iostream>

ComputeShader& ComputeShader::Use() {
    glUseProgram(this->ID);
    return *this;
}

void ComputeShader::Compile(const char* source) {
    GLuint sCompute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(sCompute, 1, &source, NULL);
    glCompileShader(sCompute);
    checkCompileErrors(sCompute, "COMPUTE");

    this->ID = glCreateProgram();
    glAttachShader(this->ID, sCompute);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "CPROGRAM");

    glDeleteShader(sCompute);
}

void ComputeShader::checkCompileErrors(GLuint object, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "CPROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::" << type << "::COMPILE_TIME_ERROR" << std::endl;
            std::cout << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::" << type << "::LINK_TIME_ERROR" << std::endl;
            std::cout << infoLog << std::endl;
        }
    }
}

// Utilitary functions

void ComputeShader::SetFloat(const char* name, float value, bool useShader) {
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void ComputeShader::SetInteger(const char* name, int value, bool useShader) {
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void ComputeShader::SetVector2f(const char* name, float x, float y, bool useShader) {
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void ComputeShader::SetVector2f(const char* name, const glm::vec2& value, bool useShader) {
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}

void ComputeShader::SetVector3f(const char* name, float x, float y, float z, bool useShader) {
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void ComputeShader::SetVector3f(const char* name, const glm::vec3& value, bool useShader) {
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}

void ComputeShader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader) {
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}

void ComputeShader::SetVector4f(const char* name, const glm::vec4& value, bool useShader) {
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}

void ComputeShader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader) {
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}