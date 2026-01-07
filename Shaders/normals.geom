#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform float normalLength = 0.10; 

in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
} data_in[];

out vec3 geom_color; 

void main()
{
    for(int i = 0; i < 3; i++)
    {
      
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        geom_color = data_in[i].color; 
        EmitVertex();

       
        gl_Position = data_in[i].projection * 
                     (gl_in[i].gl_Position + normalLength * vec4(data_in[i].Normal, 0.0f));
        geom_color = data_in[i].color;
        EmitVertex();

        EndPrimitive();
    }
}