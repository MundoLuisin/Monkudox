#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 camMatrix;
uniform float outlineWidth;  // lo llamo outlineWidth para que quede claro

void main()
{
    // Desplazamos la posición por la normal, pero EN ESPACIO LOCAL (antes de model)
    vec3 offsetPos = aPos + aNormal * outlineWidth;

    // Aplicamos la matriz model completa (incluye translate, rotate, scale)
    vec4 worldPos = model * vec4(offsetPos, 1.0);

    gl_Position = camMatrix * worldPos;
}