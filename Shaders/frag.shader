#version 460 core

flat in uint fragMaterialIndex;
in vec3 fragNormal;
in vec3 fragWorldPos;

uniform vec3 lightDir;   // direction VERS la lumière, normalisée, en espace monde
uniform vec3 lightColor;
uniform vec3 baseColor;  // à terme, remplacer par un lookup matériau via fragMaterialIndex
in vec3 fragMeshCol;
out vec4 outColor;

void main()
{
    vec3 N = normalize(fragNormal);
    float NdotL = max(dot(N, normalize(lightDir)), 0.0);

    vec3 ambient = fragMeshCol * 0.15;
    vec3 diffuse = fragMeshCol * lightColor * NdotL;

    outColor = vec4(ambient + diffuse, 1.0);
}
