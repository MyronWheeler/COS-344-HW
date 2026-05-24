#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3      objectColor;
uniform float     objectAlpha;
uniform sampler2D objectTexture;
uniform int       useTexture;

uniform int       useAlpha;     

uniform sampler2D shadowMap;    

uniform vec3 dirLightDirection; 
uniform vec3 dirLightColor;

uniform vec3 pointLightPositions[18];
uniform vec3 pointLightColor;
uniform int  numPointLights;

uniform bool  spotlightOn;
uniform vec3  spotlightPos;
uniform vec3  spotlightDir;
uniform float spotlightCutoff;       
uniform float spotlightOuterCutoff;  

uniform bool isNight;
uniform vec3 viewPos;

float calcShadow(vec4 fragPosLS, vec3 norm, vec3 toLight) {
    vec3 proj = fragPosLS.xyz / fragPosLS.w;
    proj = proj * 0.5 + 0.5;

    if (proj.z > 1.0) return 0.0;

    float bias    = max(0.05 * (1.0 - dot(norm, toLight)), 0.005);
    float shadow  = 0.0;
    vec2  texel   = vec2(1.0 / 2048.0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float closest = texture(shadowMap,
                                    proj.xy + vec2(float(x), float(y)) * texel).r;
            shadow += (proj.z - bias > closest) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 calcDirLight(vec3 norm, vec3 viewDir, vec3 base, float shadow) {
    vec3 toLight  = normalize(-dirLightDirection);
    float diff    = max(dot(norm, toLight), 0.0);
    vec3 halfV    = normalize(toLight + viewDir);
    float spec    = pow(max(dot(norm, halfV), 0.0), 64.0);

    float ambStr  = isNight ? 0.06 : 0.35;
    float diffStr = isNight ? 0.35 : 0.75;
    float specStr = isNight ? 0.05 : 0.35;

    vec3 ambient  = ambStr  * dirLightColor * base;
    vec3 diffuse  = diffStr * diff * dirLightColor * base;
    vec3 specular = specStr * spec * dirLightColor;

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calcPointLight(vec3 lPos, vec3 norm, vec3 viewDir, vec3 base, vec3 lColor) {
    vec3  toLight = normalize(lPos - FragPos);
    float diff    = max(dot(norm, toLight), 0.0);
    vec3  halfV   = normalize(toLight + viewDir);
    float spec    = pow(max(dot(norm, halfV), 0.0), 32.0);

    float d   = length(lPos - FragPos);
    float att = 1.0 / (1.0 + 0.045 * d + 0.008 * d * d);

    vec3 ambient  = 0.05  * lColor * base;
    vec3 diffuse  = 0.80  * diff * lColor * base;
    vec3 specular = 0.60  * spec * lColor;

    return (ambient + diffuse + specular) * att;
}

vec3 calcSpotlight(vec3 norm, vec3 viewDir, vec3 base) {
    vec3  toLight   = normalize(spotlightPos - FragPos);
    float theta     = dot(toLight, normalize(-spotlightDir));
    float innerCos  = cos(radians(spotlightCutoff));
    float outerCos  = cos(radians(spotlightOuterCutoff));
    float intensity = smoothstep(outerCos, innerCos, theta);

    if (intensity <= 0.0) return vec3(0.0);

    float diff   = max(dot(norm, toLight), 0.0);
    vec3  halfV  = normalize(toLight + viewDir);
    float spec   = pow(max(dot(norm, halfV), 0.0), 64.0);

    float d   = length(spotlightPos - FragPos);
    float att = 1.0 / (1.0 + 0.007 * d + 0.002 * d * d);

    vec3 diffuse  = diff * base;
    vec3 specular = 0.6 * spec * vec3(1.0);

    return (diffuse + specular) * att * intensity;
}

void main() {
    vec3 base = (useTexture == 1)
        ? texture(objectTexture, TexCoord).rgb
        : objectColor;

    vec3 norm    = normalize(Normal);
    if (norm.y < 0.0) norm = -norm;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 toLight = normalize(-dirLightDirection);

    float shadow = calcShadow(FragPosLightSpace, norm, toLight);

    vec3 result = calcDirLight(norm, viewDir, base, shadow);

    float lightScale = isNight ? 1.0 : 0.3;
    vec3  scaledPL   = pointLightColor * lightScale;
    for (int i = 0; i < numPointLights; ++i)
        result += calcPointLight(pointLightPositions[i], norm, viewDir, base, scaledPL);

    if (spotlightOn)
        result += calcSpotlight(norm, viewDir, base);

    float alpha = (useAlpha == 1) ? objectAlpha : 1.0;
    FragColor = vec4(result, alpha);
}
