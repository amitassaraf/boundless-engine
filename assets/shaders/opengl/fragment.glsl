#version 330 core

flat in vec3 out_Nom;
in vec3 out_Pos;
in vec3 view_Pos;  

out vec4 FragColor;

uniform vec3 lightPos;  
uniform vec3 viewPos;

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 0.9f);
    float ambientStrength = 0.15;
    float specularStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(out_Nom);
    vec3 lightDir = normalize(-lightPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - out_Pos);
    vec3 reflectDir = reflect(-lightDir, norm);  

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 color = (ambient + diffuse + specular) * vec3(1.0f);;
    FragColor = vec4(color, 1.0f);
}
