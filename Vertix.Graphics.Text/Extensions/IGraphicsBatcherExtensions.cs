using FreeTypeSharp;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using Vertix.Graphics.Resources;

namespace Vertix.Graphics.Text.Extensions;

public static class IGraphicsBatcherExtensions
{
    extension(IGraphicsBatcher<Vertex2D.InstanceTransform2D> graphicsBatcher)
    {
        public unsafe void DrawText(string text, Vector2D<float> position, FontFamily fontFamily,
            uint fontSize = 16, FontStyle fontStyle = FontStyle.Normal, uint? lineSpacing = null)
        {
            List<FontTextureAtlas> fontTextures;
            lineSpacing ??= (uint)(fontSize / 4f);

            if (!fontFamily.FontStyles.Contains(fontStyle)) fontStyle = FontStyle.Normal;
            if (!fontFamily._fontTextures.TryGetValue(fontStyle, out fontTextures!))
            {
                fontTextures = [];
                fontFamily._fontTextures[fontStyle] = fontTextures;
            }

            float scale = fontSize / (float)FontTextureAtlas.SDF_FONT_PIXEL_SIZE;
            ITexture2D? texture2D = null;
            ReadOnlySpan<char> chars = text.AsSpan();
            Vector2D<float> offset = Vector2D<float>.Zero;

            for (int i = 0; i < chars.Length; i++)
            {
                char character = chars[i];

                if (character == '\n')
                {
                    offset.X = 0;
                    offset.Y += fontSize + lineSpacing.Value;
                    continue;
                }
                if (char.IsControl(character))
                    continue;

                if (!fontFamily._glyphsIndexes.TryGetValue((character, fontStyle), out int textureIndex))
                {
                    if (fontTextures.Count == 0 || fontTextures[^1].IsFull)
                        fontTextures.Add(new((FT_FaceRec_*)fontFamily._faces[fontStyle], graphicsBatcher.GraphicsDevice));

                    textureIndex = fontTextures.Count - 1;

                    if (!fontTextures[^1].Draw(character))
                        throw new Exception($"Failed to draw character '{character}' in font style '{fontStyle}'.");
                    fontFamily._glyphsIndexes[(character, fontStyle)] = fontTextures.Count - 1;
                }

                int lineHeight = fontTextures[textureIndex].LineHeight;
                FontGlyph fontGlyph = fontTextures[textureIndex].Glyphs[character];
                if (texture2D != fontTextures[textureIndex].Texture)
                {
                    if (texture2D != null) graphicsBatcher.Flush();

                    texture2D = fontTextures[textureIndex].Texture;
                    texture2D.BindTexture(0);
                }

                graphicsBatcher.DrawInstance(new()
                {
                    Position = position + offset + new Vector2D<float>(fontGlyph.Bearing.X, lineHeight - fontGlyph.Bearing.Y) * scale,
                    Size = new Vector2D<float>(fontGlyph.Size.X, fontGlyph.Size.Y) * scale,
                    TextureOffset = fontGlyph.UVTopLeft,
                    TextureScale = fontGlyph.UVBottomRight - fontGlyph.UVTopLeft
                });

                offset += new Vector2D<float>(fontGlyph.Advance * scale, 0);
            }

            graphicsBatcher.Flush();
        }

    }
}
