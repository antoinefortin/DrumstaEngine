#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../Datas/DatasType.h"   // pour Mesh / Vertex

struct aiScene;
struct aiNode;
struct aiMesh;

class AssetImporter
{
public:
    AssetImporter();

    void LoadAsset(
        const std::string& path,
        std::vector<Mesh>& _tmp,
        std::vector<glm::mat4x4>& transforms,
        std::vector<DrawColor>& drawColor,
        std::vector<MaterialData>& materials,
        std::vector<ImageData>& images
    );
    bool loadSceneInMemory(const std::string& path);

    void processNode(
        const aiNode* node,
        const aiScene* scene,
        const glm::mat4& parentXf,
        std::vector<Mesh>& _tmp,
        std::vector<glm::mat4x4>& transforms,
        std::vector<DrawColor>& drawColor
    );

    void processMesh(
        aiMesh* mesh,
        const aiScene* scene,
        const glm::mat4& worldXf,
        std::vector<Mesh>& _tmp,
        std::vector<glm::mat4x4>& transforms,
        std::vector<DrawColor>& drawColor
    );

    void processMaterials(
        const aiScene* scene,
        const std::string& assetPath,
        std::vector<MaterialData>& materials,
        std::vector<ImageData>& images
    );

};
