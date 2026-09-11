#version 460 core

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
};

struct DrawMetadata {
    uint baseVertex;
    uint materialIndex;
    uint padding0;
    uint padding1;
};

struct DrawColor {
    float r, g, b, padding;
};


layout(std430, binding = 0) readonly buffer VertexBuffer    { Vertex vertices[]; };
layout(std430, binding = 1) readonly buffer IndexBuffer     { uint indices[]; };
layout(std430, binding = 2) readonly buffer TransformBuffer { mat4 transforms[]; };
layout(std430, binding = 3) readonly buffer MetadataBuffer  { DrawMetadata metadata[]; };
layout(std430, binding = 4) readonly buffer ColorBuffer  { DrawColor colors[]; };

uniform mat4 viewProj;

flat out uint fragMaterialIndex;
out vec3 fragNormal;
out vec3 fragWorldPos;
out vec3 fragMeshCol;

void main()
{

    // Query SSBos based on draecall numerb
    DrawMetadata meta = metadata[gl_DrawID]; 
    DrawColor c = colors[gl_DrawID];
    fragMeshCol = vec3(c.r, c.g, c.b);

    uint localIndex    = indices[gl_VertexID];
    uint absoluteIndex = localIndex + meta.baseVertex;

    Vertex v = vertices[absoluteIndex];
    vec3 position = vec3(v.x, v.y, v.z);
    vec3 normal   = vec3(v.nx, v.ny, v.nz);

    mat4 model = transforms[gl_BaseInstance + gl_InstanceID];
    mat3 normalMatrix = mat3(transpose(inverse(model)));

    vec4 worldPos = model * vec4(position, 1.0);
    gl_Position = viewProj * worldPos;

    fragMaterialIndex = meta.materialIndex;
    fragNormal   = normalize(normalMatrix * normal);
    fragWorldPos = worldPos.xyz;
}
