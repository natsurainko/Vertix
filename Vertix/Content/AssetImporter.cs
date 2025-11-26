using Silk.NET.Assimp;
using Silk.NET.Maths;
using SkiaSharp;
using System;
using System.Collections.Generic;
using Vertix.Graphics;
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

    #region Texture Import

    public ITexture2D LoadImageTexture(IGraphicsDevice graphicsDevice, string filePath, TextureFormat textureFormat = TextureFormat.Bgra8)
    {
        ITexture2D texture = graphicsDevice.CreateTexture2D();

        using SKCodec sKCodec = SKCodec.Create(filePath);
        SKImageInfo sKImageInfo = sKCodec.Info;

        sKImageInfo.ColorType = textureFormat switch
        {
            TextureFormat.R8 => SKColorType.R8Unorm,
            //TextureFormat.R16 => SKColorType.Alpha16,

            TextureFormat.RG8 => SKColorType.Rg88,
            TextureFormat.RG16 => SKColorType.Rg1616,

            //TextureFormat.Rgb8 => SKColorType.Rgb888x,
            //TextureFormat.Bgr8 => SKColorType.Bgr101010x,

            TextureFormat.Rgba8 => SKColorType.Rgba8888,
            TextureFormat.Bgra8 => SKColorType.Bgra8888,
            TextureFormat.Rgba16 => SKColorType.Rgba16161616,
            //TextureFormat.Bgra16 => SKColorType.Rgba16161616,

            //TextureFormat.Srgb8 => SKColorType.Srgba8888,
            TextureFormat.Srgb8Alpha8 => SKColorType.Srgba8888,

            //TextureFormat.R16f => SKColorType.AlphaF16,
            TextureFormat.RG16f => SKColorType.RgF16,

            //TextureFormat.Rgb16f => SKColorType.RgbaF16,
            //TextureFormat.Rgb32f => SKColorType.RgbaF32,

            TextureFormat.Rgba16f => SKColorType.RgbaF16,
            TextureFormat.Rgba32f => SKColorType.RgbaF32,

            _ => throw new NotSupportedException($"TextureFormat {textureFormat} is not supported")
        };
        sKImageInfo.AlphaType = textureFormat switch
        {
            TextureFormat.R8 => SKAlphaType.Opaque,
            TextureFormat.RG8 => SKAlphaType.Opaque,
            TextureFormat.RG16 => SKAlphaType.Opaque,
            TextureFormat.RG16f => SKAlphaType.Opaque,
            _ => sKImageInfo.AlphaType,
        };

        using SKBitmap bitmap = SKBitmap.Decode(sKCodec, sKImageInfo);
        Vector2D<uint> size = new((uint)bitmap.Width, (uint)bitmap.Height);

        texture.Initialize(size, textureFormat);
        texture.SetData(size, Vector2D<int>.Zero, bitmap.GetPixelSpan());

        return texture;
    }

    #endregion

    public void Dispose()
    {
        _assimp.Dispose();
    }
}
