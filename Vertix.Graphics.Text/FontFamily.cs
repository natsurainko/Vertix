using FreeTypeSharp;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using static FreeTypeSharp.FT;

namespace Vertix.Graphics.Text;

public unsafe partial struct FontFamily : IDisposable
{
    internal readonly Dictionary<FontStyle, IntPtr> _faces = [];
    internal readonly Dictionary<FontStyle, List<FontTextureAtlas>> _fontTextures = [];

    internal readonly Dictionary<(char, FontStyle), int> _glyphsIndexes = [];

    public string? Name { get; }

    public FontStyle[] FontStyles { get; }

    public FontFamily(string fontFilePath)
    {
        byte* bytes = null;

        try
        {
            bytes = (byte*)Marshal.StringToHGlobalAnsi(fontFilePath);

            FT_FaceRec_* face;
            uint faceCount = 1;

            _ = FT_New_Face(FT_Lib, bytes, -1, &face);
            faceCount = (uint)face->num_faces;

            FontStyles = new FontStyle[faceCount];

            for (int i = 0; i < face->num_faces; i++)
            {
                FT_FaceRec_* currentFace;
                FT_Error error = FT_New_Face(FT_Lib, bytes, i, &currentFace);

                if (error != FT_Error.FT_Err_Ok)
                {
                    FT_Done_Face(currentFace);
                    continue;
                }

                string? familyName = Marshal.PtrToStringUTF8((nint)currentFace->family_name);
                FontStyles[i] = (FontStyle)(int)currentFace->style_flags;

                if (Name == null && familyName != null)
                    Name = familyName;

                _faces[FontStyles[i]] = (nint)currentFace;
            }

            FT_Done_Face(face);
        }
        finally
        {
            if (bytes != null)
                Marshal.FreeHGlobal((nint)bytes);
        }
    }

    /*
    public void DrawText(IGraphicsBatcher<Vertex2D.InstanceTransform2D> graphicsBatcher, string text, Vector2D<float> position,
        uint fontSize = 16, FontStyle fontStyle = FontStyle.Normal)
    {
        List<FontTextureAtlas> fontTextures;

        if (!FontStyles.Contains(fontStyle)) fontStyle = FontStyle.Normal;
        if (!_fontTextures.TryGetValue(fontStyle, out fontTextures!))
        {
            fontTextures = [];
            _fontTextures[fontStyle] = fontTextures;
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
                offset.Y += fontSize;
                continue;
            }

            if (!_glyphsIndexes.TryGetValue((character, fontStyle), out int textureIndex))
            {
                if (fontTextures.Count == 0 || fontTextures[^1].IsFull)
                    fontTextures.Add(new((FT_FaceRec_*)_faces[fontStyle], graphicsBatcher.GraphicsDevice));

                textureIndex = fontTextures.Count - 1;

                if (!fontTextures[^1].Draw(character))
                    throw new Exception($"Failed to draw character '{character}' in font style '{fontStyle}'.");
                _glyphsIndexes[(character, fontStyle)] = fontTextures.Count - 1;
            }

            FontGlyph fontGlyph = fontTextures[textureIndex].Glyphs[character];
            if (texture2D != fontTextures[textureIndex].Texture)
            {
                if (texture2D != null) graphicsBatcher.Flush();

                texture2D = fontTextures[textureIndex].Texture;
                texture2D.BindTexture(0);
            }

            graphicsBatcher.DrawInstance(new()
            {
                Position = position + offset + new Vector2D<float>(fontGlyph.Bearing.X, FontTextureAtlas.SDF_FONT_PIXEL_SIZE - fontGlyph.Bearing.Y) * scale,
                Size = new Vector2D<float>(fontGlyph.Size.X, fontGlyph.Size.Y) * scale,
                TextureOffset = fontGlyph.UVTopLeft,
                TextureScale = fontGlyph.UVBottomRight - fontGlyph.UVTopLeft
            });

            offset += new Vector2D<float>(fontGlyph.Advance * scale, 0);
        }

        graphicsBatcher.Flush();
    }
    */

    public readonly void Dispose()
    {
        foreach (var facePtr in _faces.Values)
        {
            FT_FaceRec_* face = (FT_FaceRec_*)facePtr;
            FT_Done_Face(face);
        }

        _faces.Clear();

        foreach (var atlasList in _fontTextures.Values)
        {
            foreach (var atlas in atlasList)
            {
                atlas.Dispose();
            }
        }

        _fontTextures.Clear();
        _glyphsIndexes.Clear();
    }
}

unsafe partial struct FontFamily
{
    internal static FT_LibraryRec_* FT_Lib;

    static FontFamily()
    {
        fixed (FT_LibraryRec_** libPtr = &FT_Lib)
        {
            FT_Error error = FT_Init_FreeType(libPtr);
            if (error != FT_Error.FT_Err_Ok)
                throw new Exception($"Failed to initialize FreeType library. Error code: {error}");
        }
    }
}