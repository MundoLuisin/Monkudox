#pragma once

#include <glm/glm.hpp>
#include "shaderClass.h"
#include "Texture.h"

enum RenderingMode
{
    OPAQUE,
    CUTOUT,
    FADE,
    TRASPARENT
};

class Material
{
public:

    Material(Shader* sh) : shader(sh) {}

    RenderingMode renderingMode = OPAQUE;

    Texture* AlbedoMap;
    glm::vec4 AlbedoColor = glm::vec4(1.0f);

    Texture* roughnessMap;
    float roughnessValue;

    Texture* metallicMap;
    float metallicValue;

    Texture* normalMap;

    Texture* heightlMap;

    Texture* ambientOclusionMap;

    Texture* emissionMap;
    float emissionValue;

    glm::vec2 tiling = glm::vec2(1.0f);
    glm::vec2 offset = glm::vec2(0.0f);

private:

    Shader* shader;
};