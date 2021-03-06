#version 330 core

flat in vec3 v_Nom;
in vec3 v_Pos;  

out vec4 FragColor;

uniform vec3 lightPos;  

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 0.9f);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(v_Nom);
    vec3 lightDir = normalize(lightPos - v_Pos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 color = (ambient + diffuse) * vec3(1.0f);
    FragColor = vec4(color, 1.0f);
}
