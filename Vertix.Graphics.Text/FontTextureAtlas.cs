using FreeTypeSharp;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using static FreeTypeSharp.FT;
using static FreeTypeSharp.FT_LOAD;
using static FreeTypeSharp.FT_Render_Mode_;

namespace Vertix.Graphics.Text;

public unsafe class FontTextureAtlas : IDisposable
{
    public const int SDF_FONT_PIXEL_SIZE = 64;

    private const int ATLAS_WIDTH = 2048;
    private const int ATLAS_HEIGHT = 2048;

    private readonly ITexture2D _texture2D;
    private readonly IGraphicsDevice _graphicsDevice;
    private readonly FT_FaceRec_* _ft_face;

    private Vector2D<int> _currentPosition = Vector2D<int>.Zero;

    public ITexture2D Texture => _texture2D;

    public bool IsFull { get; private set; }

    public int LineHeight { get; private set; }

    public Dictionary<char, FontGlyph> Glyphs { get; } = [];

    public FontTextureAtlas(FT_FaceRec_* ft_face, IGraphicsDevice graphicsDevice)
    {
        _ft_face = ft_face;
        _graphicsDevice = graphicsDevice;

        _texture2D = graphicsDevice.CreateTexture2D();
        _texture2D.Initialize(new(ATLAS_WIDTH, ATLAS_HEIGHT), TextureFormat.R8);

        FT_Set_Pixel_Sizes(_ft_face, 0, SDF_FONT_PIXEL_SIZE);
        LineHeight = (int)(_ft_face->size->metrics.height >> 6);
    }

    public bool Draw(char character)
    {
        if (Glyphs.ContainsKey(character)) return true;

        uint glyphIndex = FT_Get_Char_Index(_ft_face, character);
        if (glyphIndex == 0) return false;

        FT_Error error = FT_Load_Glyph(_ft_face, glyphIndex, FT_LOAD_DEFAULT);
        if (error != FT_Error.FT_Err_Ok) return false;
        FT_GlyphSlotRec_* glyph = _ft_face->glyph;

        error = FT_Render_Glyph(_ft_face->glyph, FT_RENDER_MODE_SDF);
        if (error != FT_Error.FT_Err_Ok) return false;

        FT_Bitmap_ bitmap = glyph->bitmap;
        uint glyphWidth = bitmap.width;
        uint glyphHeight =  bitmap.rows;

        if (_currentPosition.X + glyphWidth > ATLAS_WIDTH)
        {
            _currentPosition.X = 0;
            _currentPosition.Y += LineHeight;
        }

        LineHeight = Math.Max(LineHeight, (int)glyphHeight);

        Span<byte> bitmapSpan = new(bitmap.buffer, (int)(glyphWidth * glyphHeight));
        FontGlyph fontGlyph = new()
        {
            Bearing = new(glyph->bitmap_left, glyph->bitmap_top),
            Size = new(glyphWidth, glyphHeight),
            Advance = glyph->advance.x >> 6,
            UVTopLeft = new
            (
                _currentPosition.X / (float)ATLAS_WIDTH,
                _currentPosition.Y / (float)ATLAS_HEIGHT
            ),
            UVBottomRight = new
            (
                (_currentPosition.X + glyphWidth) / (float)ATLAS_WIDTH, 
                (_currentPosition.Y + glyphHeight) / (float)ATLAS_HEIGHT
            )
        };

        _texture2D.SetData(fontGlyph.Size, _currentPosition, bitmapSpan);

        _currentPosition.X += (int)glyphWidth;
        Glyphs[character] = fontGlyph;

        if (ATLAS_WIDTH < SDF_FONT_PIXEL_SIZE + _currentPosition.X && 
            ATLAS_HEIGHT < SDF_FONT_PIXEL_SIZE + _currentPosition.Y)
            IsFull = true;

        return true;
    }

    public void Dispose()
    {
        _texture2D.Dispose();
        Glyphs.Clear();
    }
}
