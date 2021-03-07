#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Norm;

flat out vec3 v_Nom;
out vec3 v_Pos;  

uniform mat4 modelScale;
uniform mat4 modelTrans;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 position = vec4(a_Pos, 1.0);
    v_Nom = mat3(transpose(inverse(modelTrans * modelScale))) * a_Norm; 
    v_Pos = vec3(modelTrans * modelScale * position);
    gl_Position = projection * view * modelTrans * modelScale * position;
}
