#version 460 core

uniform sampler2D uBaseColor;
uniform int uHasTexture;

in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec3 fragMeshCol;
flat in uint fragMaterialIndex;
flat in uint fragTextureIndex;

out vec4 outColor;

void main()
{
    vec3 baseColor = fragMeshCol;
    if (uHasTexture != 0)
        baseColor = texture(uBaseColor, fragUV).rgb;

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(vec3(0.4, 1.0, 0.3));
    float light = max(dot(N, L), 0.0) * 0.8 + 0.2;

    outColor = vec4(baseColor * light, 1.0);
}
