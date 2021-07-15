#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>
#include <memory>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh {
public:
    GLuint VAO;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Mesh(const char* path);
    ~Mesh();

private:
    GLuint VBO, EBO;
    void loadFromFile(const char* path);
    void setupMesh();
};

Mesh::Mesh(const char* path) {
    loadFromFile(path);
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::loadFromFile(const char* path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | 
                                                   aiProcess_GenNormals);

    if((!scene) || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || (!scene->mRootNode)) {
        std::cout << "ERROR::MESH::FAILED_TO_LOAD_FILE" << std::endl;
        return;
    }
    
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
        {
            Vertex tmpVertex;

            tmpVertex.position.x = mesh->mVertices[j].x;
            tmpVertex.position.y = mesh->mVertices[j].y;
            tmpVertex.position.z = mesh->mVertices[j].z;

            tmpVertex.normal.x   = mesh->mNormals[j].x;
            tmpVertex.normal.y   = mesh->mNormals[j].y;
            tmpVertex.normal.z   = mesh->mNormals[j].z;

            vertices.push_back(tmpVertex);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; ++k)
                indices.push_back(face.mIndices[k]);
        }
    }
}                           



void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex,normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}