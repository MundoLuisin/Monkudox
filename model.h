#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "ShaderClass.h"

class Model
{
public:
    std::vector<Mesh> meshes;
    std::string directory;

    Model(const char* path)
    {
        loadModel(path);
    }

    void Draw(Shader& shader, Camera& camera, glm::mat4 modelMatrix)
    {
        for (auto& mesh : meshes)
        {
            mesh.Draw(shader, camera, modelMatrix);  // pasa la matriz a cada mesh
        }
    }     

private:
    const aiScene* m_Scene;
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform = glm::mat4(1.0f));
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};