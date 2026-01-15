#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec4 fragPosLight;

out DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
} data_out;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 lightProjection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0f);
    crntPos = worldPos.xyz;
    Normal = mat3(model) * aNormal;
    color = aColor;
    texCoord = vec2(aTex.x, 1.0 - aTex.y);

    fragPosLight = lightProjection * vec4(crntPos, 1.0f);

    gl_Position = worldPos;  // Output world-space pos (geom will project it)

    data_out.Normal = Normal;  // Use world-space to match direct out
    data_out.color = color;
    data_out.texCoord = texCoord;  // Use standard flip to match direct out (remove the weird mat2)
    data_out.projection = camMatrix;
}