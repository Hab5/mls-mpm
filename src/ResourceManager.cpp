#include "ResourceManager.hpp"

#include <stb/stb_image.h>

#include <iostream>
#include <sstream>
#include <fstream>

std::map<std::string, Shader>        ResourceManager::Shaders;
std::map<std::string, ComputeShader> ResourceManager::ComputeShaders;

Shader ResourceManager::LoadShader(std::string name,
                                   const char* vShaderFile,
                                   const char* fShaderFile,
                                   const char* gShaderFile)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

ComputeShader ResourceManager::LoadComputeShader(std::string name,
                                                 const char* cShaderFile)
{
    ComputeShaders[name] = loadComputeShaderFromFile(cShaderFile);
    return ComputeShaders[name];
}

ComputeShader ResourceManager::GetComputeShader(std::string name) {
    return ComputeShaders[name];
}

void ResourceManager::CleanUp() {
    for (auto shader: Shaders)
        glDeleteProgram(shader.second.ID);
    for (auto computeShader: ComputeShaders)
        glDeleteProgram(computeShader.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile,
                                           const char* fShaderFile,
                                           const char* gShaderFile)
{
    // Read Shaders' source
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    
    try {
        // Open Files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        if (!vertexShaderFile || !fragmentShaderFile)
            throw "FILE_DO_NOT_EXIST";
        std::stringstream vShaderStream, fShaderStream;
        // File content to stream
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // Close file handlders
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Stream to string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // Same for geometry shader if needed.
        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR::SHADER::FAILED_TO_READ_FILE" << std::endl;
    } catch (const char* e) {
        std::cout << "ERROR::SHADER::" << e << std::endl;
    }

    // C string from std::string
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();

    // Compile shader
    Shader shader;
    shader.Compile(vShaderCode, 
                   fShaderCode, 
                   gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

ComputeShader ResourceManager::loadComputeShaderFromFile(const char* cShaderFile) {
    std::string computeCode;
    
    try {
        // Open Files
        std::ifstream computeShaderFile(cShaderFile);
        if (!computeShaderFile)
            throw "FILE_DO_NOT_EXIST";
        std::stringstream cShaderStream;
        // File content to stream
        cShaderStream << computeShaderFile.rdbuf();
        // Close file handlder
        computeShaderFile.close();
        // Stream to string
        computeCode = cShaderStream.str();
        // Same for geometry shader if needed.
    } catch (const std::exception& e) {
        std::cout << "ERROR::SHADER::FAILED_TO_READ_FILE" << std::endl;
    } catch (const char* e) {
        std::cout << "ERROR::SHADER::" << e << std::endl;
    }

    // C string from std::string
    const char* cShaderCode = computeCode.c_str();

    // Compile shader
    ComputeShader computeShader;
    computeShader.Compile(cShaderCode);
    return computeShader;
}