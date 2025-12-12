using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;

namespace Vertix.OpenGL.Graphics;

public partial class GLTexture2DArray : GLTexture, ITexture2DArray
{
    private PixelFormat _pixelFormat;
    private PixelType _pixelType;

    public Vector2D<uint> Size { get; private set; }

    public uint Layers { get; private set; } = 1;

    public GLTexture2DArray(GL gL) : base(gL, gL.CreateTexture(TextureTarget.Texture2DArray)) { }

    public GLTexture2DArray(GL gL, uint handle) : base(gL, handle) { }

    public void Initialize(Vector2D<uint> size, uint layers, TextureFormat format, uint mipmapLevels = 1)
    {
        TextureFormat = format;
        MipmapLevels = mipmapLevels;
        TextureFormat.ToPixelFormatAndType(out _pixelFormat, out _pixelType);
        _gL.TextureStorage3D(Handle, MipmapLevels, TextureFormat.ToGLSizedInternalFormat(), size.X, size.Y, layers);
        Size = size;
        Layers = layers;
        Initialized = true;
    }

    public void SetData<TData>(Vector2D<uint> size, int layerIndex, Vector2D<int> offset, ReadOnlySpan<TData> data, int mipmapIndex = 0)
        where TData : unmanaged
    {
        _gL.GetInteger(GetPName.UnpackAlignment, out int oldAlignment);
        _gL.PixelStore(PixelStoreParameter.UnpackAlignment, 1);

        _gL.TextureSubImage3D
        (
            Handle, mipmapIndex,
            offset.X, offset.Y, layerIndex,
            size.X, size.Y, 1,
            _pixelFormat,
            _pixelType,
            data
        );

        _gL.PixelStore(PixelStoreParameter.UnpackAlignment, oldAlignment);
    }
}
