using FreeTypeSharp;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using System.Numerics;
using Vertix.Extensions;
using Vertix.Graphics.Primitives;

namespace Vertix.Graphics.Text.Extensions;

public static class IGraphicsBatcherExtensions
{
    extension(IGraphicsBatcher<Vertex2D.InstanceTransform2D> graphicsBatcher)
    {
        public unsafe void DrawText(string text, Vector2 position, FontFamily fontFamily,
            uint fontSize = 16, FontStyle? targetFontStyle = default, uint? lineSpacing = null)
        {
            FontStyle fontStyle = targetFontStyle ?? fontFamily.FontStyles[0];
            List<FontTextureAtlas> fontTextures;
            lineSpacing ??= (uint)(fontSize / 4f);

            if (!fontFamily.FontStyles.Contains(fontStyle)) fontStyle = fontFamily.FontStyles[0];
            if (!fontFamily._fontTextures.TryGetValue(fontStyle, out fontTextures!))
            {
                fontTextures = [];
                fontFamily._fontTextures[fontStyle] = fontTextures;
            }

            float scale = fontSize / (float)FontTextureAtlas.SDF_FONT_PIXEL_SIZE;
            ITexture2D? texture2D = null;
            ReadOnlySpan<char> chars = text.AsSpan();
            Vector2 offset = Vector2.Zero;

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
                    graphicsBatcher.GraphicsDevice.BindTexture(0, texture2D, Rendering.ShaderType.PixelShader);
                }

                Vector2 origin = position + offset + new Vector2(fontGlyph.Bearing.X, lineHeight - fontGlyph.Bearing.Y) * scale;
                Vector2 size = new Vector2(fontGlyph.Size.X, fontGlyph.Size.Y) * scale;
                Vector4 textureRegion = new
                (
                    fontGlyph.UVTopLeft.X,
                    fontGlyph.UVTopLeft.Y,
                    fontGlyph.UVBottomRight.X - fontGlyph.UVTopLeft.X,
                    fontGlyph.UVBottomRight.Y - fontGlyph.UVTopLeft.Y
                );

                graphicsBatcher.DrawInstance(new()
                {
                    WorldMatirx = new Rectangle<float>(origin.X, origin.Y, size.X, size.Y).ToScreenMatrix(),
                    TextureRegion = textureRegion
                });

                offset += new Vector2(fontGlyph.Advance * scale, 0);
            }

            graphicsBatcher.Flush();
        }

    }
}
