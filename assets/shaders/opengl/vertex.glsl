#version 330 core

layout (location = 0) in vec3 v_Pos;
layout (location = 1) in vec3 v_Norm;
layout (location = 2) in mat4 m_ModelMatrix;

flat out vec3 out_Nom;
out vec3 out_Pos;  

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 position = vec4(v_Pos, 1.0);
    out_Nom = mat3(transpose(inverse(m_ModelMatrix))) * v_Norm; 
    out_Pos = vec3(m_ModelMatrix * position);
    gl_Position = projection * view * m_ModelMatrix * position;
}
