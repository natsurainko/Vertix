using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using Vertix.Graphics;
using Vertix.Graphics.Primitives;

namespace Vertix.Engine.Content;

public class ContentManager : IDisposable
{
    private readonly IGraphicsDevice _graphicsDevice;
    private readonly AssetImporter _assetImporter;

    private readonly Dictionary<string, ITexture> _textures = [];
    private readonly Dictionary<string, Model> _models = [];

    public IReadOnlyDictionary<string, ITexture> Textures => _textures;
    public IReadOnlyDictionary<string, Model> Models => _models;

    public ContentManager(IGraphicsDevice graphicsDevice, AssetImporter assetImporter)
    {
        _graphicsDevice = graphicsDevice;
        _assetImporter = assetImporter;

        ITexture2D texture2D = graphicsDevice.CreateTexture2D();
        texture2D.Initialize(Vector2D<uint>.One, TextureFormat.Rgba8);
        texture2D.SetData(Vector2D<uint>.One, Vector2D<int>.Zero, stackalloc byte[] { 255, 255, 255, 255 });
        _textures.Add("DefaultTexture2D", texture2D);
    }

    public void LoadManifest(string manifestPath)
    {
        string manifestContent = File.ReadAllText(manifestPath);
        ContentManifest contentManifest = JsonSerializer.Deserialize(manifestContent, SerializerContext.Default.ContentManifest)
            ?? throw new InvalidDataException();

        ITexture2D[] texture2Ds = _graphicsDevice.CreateTexture2Ds(contentManifest.Textures.Texture2dDeclarations.Length);
        for (int i = 0; i < contentManifest.Textures.Texture2dDeclarations.Length; i++)
        {
            ContentManifest.TextureDeclarations.Texture2DContentDeclaration declaration = contentManifest.Textures.Texture2dDeclarations[i];
            ITexture2D texture2D = texture2Ds[i];

            _assetImporter.LoadImageTexture(texture2D, declaration.FilePath, declaration.TextureFormat);
            _textures.Add(declaration.Key, texture2D);
        }

        for (int i = 0; i < contentManifest.ModelDeclarations.Length; i++)
        {
            ContentManifest.ModelContentDeclaration declaration = contentManifest.ModelDeclarations[i];
            Model model = _assetImporter.LoadModel(declaration.FilePath);

            model.InitializeMeshesVertexArray(_graphicsDevice);
            _models.Add(declaration.Key, model);
        }
    }

    public void Dispose()
    {
        foreach (ITexture texture in _textures.Values)
            texture.Dispose();

        foreach (Model model in _models.Values)
            model.DisposeMeshesVertexArray();
    }
}