#pragma once

#include <glad/glad.h>

#include "Shader.hpp"
#include "ComputeShader.hpp"

#include <map>
#include <string>


class ResourceManager {
public:
    // Resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, ComputeShader> ComputeShaders;
    
    // Load a shader from the given sources, also returns the object
    static Shader LoadShader(std::string name,
                             const char* vShaderFile,
                             const char* fShaderFile,
                             const char* gShaderFile=nullptr);
    // Get the specified shader
    static Shader GetShader(std::string name);

    // Load a shader from the given source, also returns the object
    static ComputeShader LoadComputeShader(std::string name,
                                           const char *cShaderFile);
    // Get the specified compute shader
    static ComputeShader GetComputeShader(std::string name);

    // Deallocate all loaded resources
    static void CleanUp();

private:
    ResourceManager() {}
    static Shader loadShaderFromFile(const char* vShaderFile,
                                     const char* fShaderFile,
                                     const char* gShaderFile=nullptr);

    static ComputeShader loadComputeShaderFromFile(const char* cShaderFile);
};