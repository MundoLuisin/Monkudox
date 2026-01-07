#version 330 core
out vec4 FragColor;
in vec2 texCoords;
uniform sampler2D screenTexture;

vec3 ACESTonemap(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x + b))/(x*(c*x + d) + e), 0.0, 1.0);
}

// ACTIVE POST PROCESING <= VIGNETTE + TONEMAPPING
void main()
{
    vec3 color = texture(screenTexture, texCoords.st).rgb;
    color *= 2.0;

    color = ACESTonemap(color);
    
    vec2 uv = texCoords.st - 0.5; 
    float vignette = smoothstep(0.9, 0.5, length(uv));  
    color *= vignette;
    
    FragColor = vec4(color, 1.0f);
}