#version 330 core

flat in vec3 out_Nom;
in vec3 out_Pos;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

void main()
{
    gPosition = out_Pos;
    gNormal = normalize(out_Nom);
    gAlbedo.rgb = vec3(0.95);
}
