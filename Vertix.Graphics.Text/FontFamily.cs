using FreeTypeSharp;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using static FreeTypeSharp.FT;

namespace Vertix.Graphics.Text;

public unsafe partial class FontFamily : IDisposable
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

            FT_Error error = FT_New_Face(FT_Lib, bytes, -1, &face);
            if (error != FT_Error.FT_Err_Ok)
                throw new Exception($"Failed to load font from file '{fontFilePath}'. Error code: {error}");

            faceCount = (uint)face->num_faces;
            FontStyles = new FontStyle[faceCount];

            for (int i = 0; i < face->num_faces; i++)
            {
                FT_FaceRec_* currentFace;
                error = FT_New_Face(FT_Lib, bytes, i, &currentFace);

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
    public void Dispose()
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

unsafe partial class FontFamily
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