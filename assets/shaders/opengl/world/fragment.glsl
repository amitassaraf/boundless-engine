#version 330 core

flat in vec3 out_Nom;
in vec3 out_Pos;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

const float near = 0.01; // projection matrix's near plane
const float far = 10000.0; // projection matrix's far plane

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - depth * (far - near));    
}

void main()
{
    gPosition = vec4(out_Pos, linearizeDepth(out_Pos.z));
    gNormal = normalize(out_Nom);
    gAlbedo.rgb = vec3(0.95);
}
