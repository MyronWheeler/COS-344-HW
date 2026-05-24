#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

void main() {
    vec2 scroll1 = TexCoord + vec2(time * 0.03,  time * 0.02);
    vec2 scroll2 = TexCoord + vec2(-time * 0.02, time * 0.03);

    vec3 n1 = texture(normalMap, scroll1).rgb * 2.0 - 1.0;
    vec3 n2 = texture(normalMap, scroll2).rgb * 2.0 - 1.0;
    vec3 norm = normalize(n1 + n2);

    vec3 lightDir  = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDirm = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);

    vec3 waterColor = vec3(0.04, 0.42, 0.78);
    vec3 ambient = 0.30 * waterColor;
    vec3 diffuse = 0.70 * diff * waterColor;
    vec3 specular = 1.20 * spec * vec3(1.0); 
    vec3 result = ambient + diffuse + specular;
    FragColor= vec4(result, 0.90);    
}
