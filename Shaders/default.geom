#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 crntPos;  // Add this to pass through (remove if frag doesn't need it)
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
} data_in[];

// Passthrough main function
void main()
{
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    crntPos = gl_in[0].gl_Position.xyz;
    Normal = data_in[0].Normal;
    color = data_in[0].color;
    texCoord = data_in[0].texCoord;
    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    crntPos = gl_in[1].gl_Position.xyz;
    Normal = data_in[1].Normal;
    color = data_in[1].color;
    texCoord = data_in[1].texCoord;
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    crntPos = gl_in[2].gl_Position.xyz;
    Normal = data_in[2].Normal;
    color = data_in[2].color;
    texCoord = data_in[2].texCoord;
    EmitVertex();

    EndPrimitive();
}

// "Explosion" main function
/*
void main()
{
    vec3 vector0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;  // Flipped order for correct cross product
    vec3 vector1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 surfaceNormalDir = normalize(cross(vector0, vector1));
    vec4 surfaceNormal = vec4(surfaceNormalDir * 0.5, 0.0);  // Add extrudeAmount (e.g., 0.5—tune this)

    gl_Position = data_in[0].projection * (gl_in[0].gl_Position + surfaceNormal);
    crntPos = (gl_in[0].gl_Position + surfaceNormal).xyz;
    Normal = data_in[0].Normal;
    color = data_in[0].color;
    texCoord = data_in[0].texCoord;
    EmitVertex();

    gl_Position = data_in[1].projection * (gl_in[1].gl_Position + surfaceNormal);
    crntPos = (gl_in[1].gl_Position + surfaceNormal).xyz;
    Normal = data_in[1].Normal;
    color = data_in[1].color;
    texCoord = data_in[1].texCoord;
    EmitVertex();

    gl_Position = data_in[2].projection * (gl_in[2].gl_Position + surfaceNormal);
    crntPos = (gl_in[2].gl_Position + surfaceNormal).xyz;
    Normal = data_in[2].Normal;
    color = data_in[2].color;
    texCoord = data_in[2].texCoord;
    EmitVertex();

    EndPrimitive();
}
*/