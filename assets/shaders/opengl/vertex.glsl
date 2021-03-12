#version 330 core

layout (location = 0) in vec3 v_Pos;
layout (location = 1) in vec3 v_Norm;
layout (location = 2) in mat4 m_Translation;
layout (location = 6) in mat4 m_Scale;

flat out vec3 out_Nom;
out vec3 out_Pos;  

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 position = vec4(v_Pos, 1.0);
    out_Nom = mat3(transpose(inverse(m_Translation * m_Scale))) * v_Norm; 
    out_Pos = vec3(m_Translation * m_Scale * position);
    gl_Position = projection * view * m_Translation * m_Scale * position;
}
