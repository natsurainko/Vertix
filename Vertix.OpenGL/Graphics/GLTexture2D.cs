using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;

namespace Vertix.OpenGL.Graphics;

public partial struct GLTexture2D : ITexture2D
{
    private readonly GL _gL;
    private readonly uint _handle;
    public readonly uint Handle => _handle;

    public bool Initialized { get; private set; }

    public Vector2D<uint> Size { get; private set; }

    public GLTexture2D(GL gL)
    {
        _gL = gL;
        _handle = _gL.CreateTexture(TextureTarget.Texture2D);
    }

    public GLTexture2D(GL gL, uint handle)
    {
        _gL = gL;
        _handle = handle;
    }

    public void Initialize(Vector2D<uint> size, uint format, uint mipmapLevels = 1)
    {
        _gL.TextureStorage2D(_handle, mipmapLevels, (SizedInternalFormat)format, size.X, size.Y);
        Size = size;
        Initialized = true;
    }

    public readonly void SetData<T>(Vector2D<uint> size, Vector2D<int> offset, uint pixelFormat, uint pixelType, 
        ReadOnlySpan<T> data, int mipmapLevel = 0) where T : unmanaged
    {
        _gL.TextureSubImage2D
        (
            _handle, mipmapLevel, 
            offset.X, offset.Y, 
            size.X, size.Y, 
            (PixelFormat)pixelFormat, 
            (PixelType)pixelType, 
            data
        );
    }

    public void Dispose()
    {
        _gL.DeleteTexture(_handle);
    }
}

public partial struct GLTexture2D
{
    public void Initialize(Vector2D<uint> size, uint mipmapLevels, SizedInternalFormat format)
    {
        _gL.TextureStorage2D(_handle, mipmapLevels, format, size.X, size.Y);
        Initialized = true;
    }

    public readonly void SetData<T>(Vector2D<uint> size, Vector2D<int> offset, PixelFormat pixelFormat, PixelType pixelType,
        ReadOnlySpan<T> data, int mipmapLevel = 0) where T : unmanaged
    {
        _gL.TextureSubImage2D
        (
            _handle, mipmapLevel,
            offset.X, offset.Y,
            size.X, size.Y,
            pixelFormat,
            pixelType,
            data
        );
    }

    //public readonly void SetParameter(TextureParameterName parameterName, GLEnum value) 
    //    => _gL.TextureParameter(_handle, parameterName, (int)value);

    public readonly void BindTexture(uint bindingIndex) => _gL.BindTextureUnit(bindingIndex, _handle);
}