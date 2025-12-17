using System.Text.Json.Serialization;
using Vertix.Graphics;

namespace Vertix.Content;

public class ContentManifest
{
    [JsonPropertyName("models")]
    public required ModelContentDeclaration[] ModelDeclarations { get; set; }

    [JsonPropertyName("textures")]
    public required TextureDeclarations Textures { get; set; }

    public class TextureDeclarations
    {
        [JsonPropertyName("texture2Ds")]
        public required Texture2DContentDeclaration[] Texture2dDeclarations { get; set; }

        public class Texture2DContentDeclaration : ContentDeclaration
        {
            [JsonPropertyName("textureFormat")]
            public required TextureFormat TextureFormat { get; set; }
        }
    }

    public class ModelContentDeclaration : ContentDeclaration;

    public class ContentDeclaration
    {
        [JsonPropertyName("key")]
        public required string Key { get; set; }

        [JsonPropertyName("filePath")]
        public required string FilePath { get; set; }
    }
}
