using Silk.NET.Assimp;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using Vertix.Graphics.Resources;

namespace Vertix.Content;

public class AssetImporter : IDisposable
{
    private readonly Assimp _assimp;

    public AssetImporter()
    {
        _assimp = Assimp.GetApi();
    }

    #region Model Import

    unsafe static void ProcessingMesh(ref Graphics.Resources.Mesh modelMesh, Silk.NET.Assimp.Mesh* mesh, Scene* scene)
    {
        for (int i = 0; i < mesh->MNumVertices; i++)
        {
            Vertex vertex = new();
            vertex.Position.X = mesh->MVertices[i].X;
            vertex.Position.Y = mesh->MVertices[i].Y;
            vertex.Position.Z = mesh->MVertices[i].Z;

            if (mesh->MNormals != null)
            {
                vertex.Normal.X = mesh->MNormals[i].X;
                vertex.Normal.Y = mesh->MNormals[i].Y;
                vertex.Normal.Z = mesh->MNormals[i].Z;
            }

            if (mesh->MTextureCoords[0] != null)
            {
                vertex.TexCoord.X = mesh->MTextureCoords[0][i].X;
                vertex.TexCoord.Y = mesh->MTextureCoords[0][i].Y;
            }
            else
                vertex.TexCoord = Vector2D<float>.Zero;

            modelMesh.Vertices[i] = vertex;
        }

        List<uint> indices = [];
        for (int i = 0; i < mesh->MNumFaces; i++)
        {
            Face face = mesh->MFaces[i];
            for (int j = 0; j < face.MNumIndices; j++)
                indices.Add(face.MIndices[j]);
        }

        modelMesh.Indices = [.. indices];
    }

    unsafe static void ProcessingNode(Scene* scene, Node* node, List<Graphics.Resources.Mesh> meshes)
    {
        for (int i = 0; i < node->MNumMeshes; i++)
        {
            Silk.NET.Assimp.Mesh* mesh = scene->MMeshes[node->MMeshes[i]];
            Graphics.Resources.Mesh modelMesh = new()
            {
                Vertices = new Vertex[mesh->MNumVertices],
                Name = mesh->MName.AsString
            };

            ProcessingMesh(ref modelMesh, mesh, scene);
            meshes.Add(modelMesh);
        }

        for (int i = 0; i < node->MNumChildren; i++)
            ProcessingNode(scene, node->MChildren[i], meshes);
    }

    public unsafe Model LoadModel(string filePath, PostProcessSteps? postProcessSteps = null)
    {
        postProcessSteps ??= 
            PostProcessSteps.Triangulate |
            PostProcessSteps.FlipUVs |
            PostProcessSteps.JoinIdenticalVertices |
            PostProcessSteps.CalculateTangentSpace;

        Scene* scene = null;

        try
        {
            scene = _assimp.ImportFile(filePath, (uint)postProcessSteps);

            Model model = new();
            List<Graphics.Resources.Mesh> meshes = [];

            ProcessingNode(scene, scene->MRootNode, meshes);
            model.Name = scene->MRootNode->MName.AsString;
            model.Meshes = [.. meshes];

            return model;
        }
        finally
        {
            if (scene != null)
                _assimp.ReleaseImport(scene);
        }
    }

    #endregion

    public void Dispose()
    {
        _assimp.Dispose();
    }
}
