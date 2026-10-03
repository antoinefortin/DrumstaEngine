#version 460 core
#extension GL_ARB_bindless_texture : require

layout(std430, binding = 5) readonly buffer TextureHandles { uvec2 textureHandles[]; };

in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec3 fragMeshCol;
flat in uint fragMaterialIndex;
flat in uint fragTextureIndex;

out vec4 outColor;

void main()
{
    vec3 baseColor;

    if (fragTextureIndex == 0xFFFFFFFFu)
    {
        baseColor = fragMeshCol; 
    } 
                                     
    else
    {
        baseColor = texture(sampler2D(textureHandles[fragTextureIndex]), fragUV).rgb;  
    
    }
    
    
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(vec3(0.4, 1.0, 0.3));
    float light = max(dot(N, L), 0.0) * 0.8 + 0.2;

    outColor = vec4(baseColor * light, 1.0);
}