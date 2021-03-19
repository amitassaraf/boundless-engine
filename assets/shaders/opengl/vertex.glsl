#version 330 core

layout (location = 0) in vec3 v_Pos;
layout (location = 1) in vec3 v_Norm;
layout (location = 2) in vec3 m_Translation;
layout (location = 3) in float m_Scale;

flat out vec3 out_Nom;
out vec3 out_Pos;  
out vec3 view_Pos;

uniform mat4 view;
uniform mat4 projection;


void main() {
    vec4 position = vec4(v_Pos, 1.0);
    
    mat4 scale = mat4(m_Scale, 0.0, 0.0, 0.0,
                    0.0, m_Scale, 0.0, 0.0, 
                    0.0, 0.0, m_Scale, 0.0, 
                    0.0, 0.0, 0.0, 1.0);

    mat4 translation = mat4(1, 0.0, 0.0, 0.0,
                            0.0, 1, 0.0, 0.0, 
                            0.0, 0.0, 1, 0.0, 
                            m_Translation.x, m_Translation.y, m_Translation.z, 1.0);

    out_Nom = mat3(transpose(inverse(translation * scale))) * v_Norm; 
    out_Pos = vec3(translation * scale * position);
    gl_Position = projection * view * translation * scale * position;
}
