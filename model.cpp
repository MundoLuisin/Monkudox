#define _CRT_SECURE_NO_WARNINGS
#include "Model.h"
#include <iostream>
#include "stb/stb_image.h"  // Ya lo usas para Texture
#include <cstring>  // para _stricmp

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices);  // (o quítalo como te dije antes)

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    this->m_Scene = scene;  // <-- GUARDAMOS EL PUNTERO
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform)
{
    // Matriz local del nodo
    glm::mat4 localTransform = glm::transpose(glm::make_mat4(&node->mTransformation.a1));

    // Matriz global = parent * local
    glm::mat4 globalTransform = parentTransform * localTransform;

    // Procesar meshes de este nodo
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
        // Si quieres aplicar la transform directamente a los vértices, puedes usar aiProcess_PreTransformVertices
        // Pero mejor pasar la matriz al draw (ver abajo)
    }

    // Recursión en hijos
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, globalTransform);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    // Vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->mNormals)
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        else
            vertex.normal = glm::vec3(0.0f);

        vertex.color = glm::vec3(1.0f);  // blanco por defecto

        if (mesh->mTextureCoords[0])
            vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.texUV = glm::vec2(0.0f);

        vertices.push_back(vertex);
    }

    // Índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Material / Texturas
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // Para PBR glTF puedes usar aiTextureType_UNKNOWN o claves específicas, pero para básico diffuse/specular funciona
    }

    return Mesh(vertices, indices, textures);
}

std::string fixPath(const std::string& path)
{
    std::string fixed = path;
    // Reemplazar \ por /
    for (size_t i = 0; i < fixed.size(); ++i)
    {
        if (fixed[i] == '\\')
            fixed[i] = '/';
    }
    return fixed;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string path = str.C_Str();

        std::string filename;

        if (!path.empty() && path[0] == '*')
        {
            // Embedded
            int embedIndex = std::stoi(path.substr(1));
            if (embedIndex >= (int)m_Scene->mNumTextures || m_Scene->mTextures[embedIndex] == nullptr)
                continue;

            aiTexture* aiTex = m_Scene->mTextures[embedIndex];

            // Mejor detección de extensión
            std::string hint(aiTex->achFormatHint);
            std::string extension = ".png";  // fallback seguro
            if (hint == "jpg" || hint == "jpeg" || hint == "JPG" || hint == "JPEG")
                extension = ".jpg";
            else if (hint == "png" || hint == "PNG")
                extension = ".png";
            else if (hint == "tga" || hint == "TGA")
                extension = ".tga";
            else if (hint == "bmp" || hint == "BMP")
                extension = ".bmp";
            else if (hint == "dds" || hint == "DDS")
                extension = ".dds";  // stbi soporta DDS básico

            // Nombre único
            filename = directory + "/__embedded_" + std::to_string(embedIndex) + "_" + std::to_string(i) + extension;

            // Si no existe, créalo
            bool exists = false;
            FILE* test = nullptr;
            errno_t err = fopen_s(&test, filename.c_str(), "rb");
            if (err == 0 && test != nullptr) {
                fclose(test);
                exists = true;
            }

            if (!exists)
            {
                FILE* f = nullptr;
                err = fopen_s(&f, filename.c_str(), "wb");
                if (f)
                {
                    size_t size = (aiTex->mHeight == 0) ? aiTex->mWidth : aiTex->mWidth * aiTex->mHeight * 4;
                    fwrite(aiTex->pcData, 1, size, f);
                    fclose(f);
                    std::cout << "Guardada textura embedded: " << filename << std::endl;
                }
                else
                {
                    std::cout << "ERROR: No se pudo guardar textura embedded" << std::endl;
                    continue;
                }
            }
        }
        else
        {
            filename = directory + "/" + fixPath(path);

            // Primero: intentar cargar desde disco
            int widthImg, heightImg, numColCh;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* bytes = stbi_load(filename.c_str(), &widthImg, &heightImg, &numColCh, 0);

            if (bytes)
            {
                // ¡ÉXITO! Cargar normal
                GLenum format = GL_RGBA;
                if (numColCh == 1) format = GL_RED;
                else if (numColCh == 3) format = GL_RGB;
                else if (numColCh == 4) format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(bytes);

                std::cout << "Textura externa cargada correctamente: " << filename << std::endl;
            }
            else
            {
                // FALLÓ externa → buscar embebida con nombre similar
                std::cout << "Textura externa no encontrada: " << filename << " → buscando versión embebida..." << std::endl;

                std::string targetBaseName = fixPath(path);
                size_t slashPos = targetBaseName.find_last_of('/');
                if (slashPos != std::string::npos)
                    targetBaseName = targetBaseName.substr(slashPos + 1);

                bool foundEmbedded = false;

                for (unsigned int e = 0; e < m_Scene->mNumTextures; e++)
                {
                    aiTexture* aiTex = m_Scene->mTextures[e];
                    if (!aiTex) continue;

                    std::string embName = aiTex->mFilename.C_Str();
                    if (embName.empty()) continue;

                    std::string embBase = embName;
                    size_t pos = embBase.find_last_of("/\\");
                    if (pos != std::string::npos)
                        embBase = embBase.substr(pos + 1);

                    if (_stricmp(embBase.c_str(), targetBaseName.c_str()) == 0)  // compara sin case
                    {
                        std::cout << "¡ENCONTRADA TEXTURA EMBEBIDA COINCIDENTE: " << embBase << "! Cargando desde memoria." << std::endl;

                        // CARGAR DESDE MEMORIA (sin guardar temporal)
                        const unsigned char* data = reinterpret_cast<const unsigned char*>(aiTex->pcData);
                        int dataSize = (aiTex->mHeight == 0) ? aiTex->mWidth : aiTex->mWidth * aiTex->mHeight * 4;

                        // Usar stbi_load_from_memory
                        bytes = stbi_load_from_memory(data, dataSize, &widthImg, &heightImg, &numColCh, 0);
                        if (bytes)
                        {
                            GLenum format = GL_RGBA;
                            if (numColCh == 1) format = GL_RED;
                            else if (numColCh == 3) format = GL_RGB;
                            else if (numColCh == 4) format = GL_RGBA;

                            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
                            glGenerateMipmap(GL_TEXTURE_2D);
                            stbi_image_free(bytes);
                            foundEmbedded = true;
                        }
                        break;
                    }
                }

                if (!foundEmbedded)
                {
                    std::cout << "No se encontró versión embebida → textura blanca." << std::endl;
                    unsigned char whitePixel[4] = { 255, 255, 255, 255 };
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
                }
            }
        }

        // INTENTAR CARGAR LA TEXTURA Y SI FALLA, CREAR UNA TEXTURA BLANCA DE 1x1
        int widthImg, heightImg, numColCh;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* bytes = stbi_load(filename.c_str(), &widthImg, &heightImg, &numColCh, 0);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (bytes)
        {
            // Éxito: cargar normal
            GLenum internalFormat = GL_RGBA;
            GLenum format = GL_RGBA;

            if (numColCh == 1) format = GL_RED;
            else if (numColCh == 3) format = GL_RGB;
            else if (numColCh == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(bytes);
        }
        else
        {
            // FALLÓ stbi_load → crear textura blanca 1x1
            std::cout << "FALLO al cargar textura: " << filename << " → usando textura blanca por defecto" << std::endl;
            unsigned char whitePixel[4] = { 255, 255, 255, 255 };
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
            // Sin mipmap para 1x1
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        // Crear el objeto Texture manualmente (sin usar tu constructor que lanza excepción)
        Texture texture;
        texture.ID = textureID;
        texture.type = typeName.c_str();;
        texture.unit = textures.size();
        texture.path = filename;  // opcional, si tienes el miembro

        textures.push_back(texture);
    }
    return textures;
}