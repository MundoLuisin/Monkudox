#version 330 core
out vec4 FragColor;
in vec2 texCoords;
uniform sampler2D screenTexture;


// ACTIVE POST PROCESING <= VIGNETTE
void main()
{
    vec3 color = texture(screenTexture, texCoords.st).rgb;
    
    vec2 uv = texCoords.st - 0.5; 
    float vignette = smoothstep(0.9, 0.5, length(uv));  
    color *= vignette;
    
    FragColor = vec4(color, 1.0f);
}