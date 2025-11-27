using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;

namespace Vertix.OpenGL.Graphics;

public abstract class GLTexture(GL gL, uint handle) : ITexture
{
    protected readonly GL _gL = gL;

    public uint Handle { get; protected set; } = handle;

    public bool Initialized { get; protected set; }

    public TextureFormat TextureFormat { get; protected set; }

    public uint MipmapLevels { get; protected set; }

    public abstract void BindTexture(uint bindingIndex);

    public abstract void Dispose();
}

public partial class GLTexture2D : GLTexture, ITexture2D
{
    private PixelFormat _pixelFormat;
    private PixelType _pixelType;

    public Vector2D<uint> Size { get; private set; }

    public GLTexture2D(GL gL) : base(gL, gL.CreateTexture(TextureTarget.Texture2D)) { }

    public GLTexture2D(GL gL, uint handle) : base(gL, handle) { }

    public void Initialize(Vector2D<uint> size, TextureFormat format, uint mipmapLevels = 1)
    {
        TextureFormat = format;
        MipmapLevels = mipmapLevels;
        TextureFormat.ToPixelFormatAndType(out _pixelFormat, out _pixelType);
        _gL.TextureStorage2D(Handle, MipmapLevels, TextureFormat.ToGLSizedInternalFormat(), size.X, size.Y);
        Size = size;
        Initialized = true;
    }

    public void SetData<TData>(Vector2D<uint> size, Vector2D<int> offset, ReadOnlySpan<TData> data, int mipmapIndex = 0) where TData : unmanaged
    {
        _gL.GetInteger(GetPName.UnpackAlignment, out int oldAlignment);
        _gL.PixelStore(PixelStoreParameter.UnpackAlignment, 1);

        _gL.TextureSubImage2D
        (
            Handle, mipmapIndex,
            offset.X, offset.Y, 
            size.X, size.Y,
            _pixelFormat,
            _pixelType, 
            data
        );

        _gL.PixelStore(PixelStoreParameter.UnpackAlignment, oldAlignment);
    }

    public override void BindTexture(uint bindingIndex) => _gL.BindTextureUnit(bindingIndex, Handle);

    public override void Dispose()
    {
        _gL.DeleteTexture(Handle);
    }
}