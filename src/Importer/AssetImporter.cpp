#include "AssetImporter.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AssetImporter::AssetImporter()
{
}

static glm::mat4 AiMatrixToGlm(const aiMatrix4x4& m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

void AssetImporter::LoadAsset(
    const std::string& path,
    std::vector<Mesh>& _tmp,
    std::vector<glm::mat4x4>& transforms,
    std::vector<DrawColor>& drawColor,
    std::vector<MaterialData>& materials,
    std::vector<ImageData>& images
)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_FlipUVs
        | aiProcess_CalcTangentSpace);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cerr << "Assimp: " << importer.GetErrorString() << "\n";
        return;
    }

    std::cout << "File loaded" << std::endl;
    std::cout << "meshes    : " << scene->mNumMeshes << "\n";
    std::cout << "materials : " << scene->mNumMaterials << "\n";
    std::cout << "textures  : " << scene->mNumTextures << "\n";
    std::cout << "root      : " << scene->mRootNode->mName.C_Str()
        << " (" << scene->mRootNode->mNumChildren << " child nodes)\n";


    processMaterials(scene, path, materials, images);

    processNode(
        scene->mRootNode,
        scene,
        glm::mat4(1.0f),
        _tmp,
        transforms,
        drawColor
    );

//    processNode(scene->mRootNode, scene, glm::mat4(1.0f), _tmp, transforms, drawColor);


}

bool AssetImporter::loadSceneInMemory(const std::string& path)
{
    return true;
}

void AssetImporter::processNode(
    const aiNode* node,
    const aiScene* scene,
    const glm::mat4& parentXf,
    std::vector<Mesh>& _tmp,
    std::vector<glm::mat4x4>& transforms,
    std::vector<DrawColor>& drawColor
)
{
    glm::mat4 localXf = AiMatrixToGlm(node->mTransformation);
    glm::mat4 worldXf = parentXf * localXf;
  //  transforms.push_back(worldXf);
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, worldXf, _tmp, transforms, drawColor);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, worldXf, _tmp, transforms, drawColor);
    }
}

void AssetImporter::processMesh(
    aiMesh* mesh,
    const aiScene* scene,
    const glm::mat4& worldXf,
    std::vector<Mesh>& _tmp,
    std::vector<glm::mat4x4>& transforms,
    std::vector<DrawColor>& drawColor
)
{
    Mesh outMesh{};

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v{};

        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;

        // Extract niormals 
        if (mesh->HasNormals())
        {
            v.nx = mesh->mNormals[i].x;
            v.ny = mesh->mNormals[i].y;
            v.nz = mesh->mNormals[i].z;
        }
  

        // UV
        if (mesh->HasTextureCoords(0))
        {
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;

        }
        // push back apres claice
        outMesh.verts.push_back(v);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            outMesh.indices.push_back(face.mIndices[j]);
        }
            
    }
    DrawColor meshcolor{ 1.0f, 1.0f, 1.0f, 0.0f };

    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        outMesh.materialId = mesh->mMaterialIndex;

        aiColor3D diffuse(1.0f, 1.0f, 1.0f);
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
        {
            meshcolor.red = diffuse.r;
            meshcolor.green = diffuse.g;
            meshcolor.blue = diffuse.b;
        }

        std::cout << "Mesh Material Index: " << mesh->mMaterialIndex
            << " Diffuse: (" << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << ")\n";
    }

    // push the data to globals
    drawColor.push_back(meshcolor);
    transforms.push_back(worldXf);
    _tmp.push_back(std::move(outMesh));
}

void AssetImporter::processMaterials(
    const aiScene* scene,
    const std::string& assetPath,
    std::vector<MaterialData>& materials,
    std::vector<ImageData>& images
)
{
    std::cout << "Procerssing material";
}
