using FreeTypeSharp;
using Silk.NET.Maths;
using System;
using System.Collections.Generic;
using Vertix.Graphics.Text.Helpers;
using static FreeTypeSharp.FT;
using static FreeTypeSharp.FT_LOAD;
using static FreeTypeSharp.FT_Render_Mode_;

namespace Vertix.Graphics.Text;

public unsafe class FontTextureAtlas : IDisposable
{
    public const int SDF_FONT_PIXEL_SIZE = 64;

    private const int ATLAS_WIDTH = 2048;
    private const int ATLAS_HEIGHT = 2048;
    private const int PADDING = 2;

    private readonly ITexture2D _texture2D;
    private readonly IGraphicsDevice _graphicsDevice;
    private readonly FT_FaceRec_* _ft_face;

    private Vector2D<int> _currentPosition = new(PADDING, PADDING);

    public ITexture2D Texture => _texture2D;

    public bool IsFull { get; private set; }

    public short LineHeight { get; }

    public Dictionary<char, FontGlyph> Glyphs { get; } = [];

    public FontTextureAtlas(FT_FaceRec_* ft_face, IGraphicsDevice graphicsDevice)
    {
        _ft_face = ft_face;
        _graphicsDevice = graphicsDevice;

        _texture2D = graphicsDevice.CreateTexture2D();
        _texture2D.Initialize(new(ATLAS_WIDTH, ATLAS_HEIGHT), TextureFormat.R8);

        FT_Set_Pixel_Sizes(_ft_face, 0, SDF_FONT_PIXEL_SIZE);
        LineHeight = _ft_face->height;
    }

    public bool Draw(char character)
    {
        if (Glyphs.ContainsKey(character)) return true;

        uint glyphIndex = FT_Get_Char_Index(_ft_face, character);
        if (glyphIndex == 0) return false;

        FT_Error error = FT_Load_Glyph(_ft_face, glyphIndex, FT_LOAD_DEFAULT);
        if (error != FT_Error.FT_Err_Ok) return false;

        error = FT_Render_Glyph(_ft_face->glyph, FT_RENDER_MODE_NORMAL);
        if (error != FT_Error.FT_Err_Ok) return false;

        FT_GlyphSlotRec_* glyph = _ft_face->glyph;
        FT_Bitmap_ bitmap = glyph->bitmap;
        uint glyphWidth = bitmap.width;
        uint glyphHeight =  bitmap.rows;

        if (_currentPosition.X + glyphWidth + PADDING > ATLAS_WIDTH)
        {
            _currentPosition.X = PADDING;
            _currentPosition.Y += LineHeight + PADDING;
        }

        Span<byte> bitmapSpan = new(bitmap.buffer, (int)(glyphWidth * glyphHeight));
        SDFHelper.GenerateSDF(ref bitmapSpan, (int)glyphWidth, (int)glyphHeight);

        FontGlyph fontGlyph = new()
        {
            Bearing = new(glyph->bitmap_left, glyph->bitmap_top),
            Size = new(glyphWidth, glyphHeight),
            Advance = glyph->advance.x / 64.0f,
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

        _currentPosition += new Vector2D<int>((int)glyphWidth + PADDING, 0);
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
