using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;

namespace Vertix.OpenGL.Graphics;

public partial struct GLTexture2D : ITexture2D
{
    private readonly GL _gL;
    private readonly uint _handle;

    private PixelFormat _pixelFormat;
    private PixelType _pixelType;

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

    public void Initialize(Vector2D<uint> size, TextureFormat format, uint mipmapLevels = 1)
    {
        format.ToPixelFormatAndType(out _pixelFormat, out _pixelType);
        _gL.TextureStorage2D(_handle, mipmapLevels, format.ToGLSizedInternalFormat(), size.X, size.Y);
        Size = size;
        Initialized = true;
    }

    public readonly void SetData<T>(Vector2D<uint> size, Vector2D<int> offset, ReadOnlySpan<T> data, int mipmapLevel = 0) where T : unmanaged
    {
        _gL.GetInteger(GetPName.UnpackAlignment, out int oldAlignment);
        _gL.PixelStore(PixelStoreParameter.UnpackAlignment, 1);

        _gL.TextureSubImage2D
        (
            _handle, mipmapLevel, 
            offset.X, offset.Y, 
            size.X, size.Y,
            _pixelFormat,
            _pixelType, 
            data
        );

        _gL.PixelStore(PixelStoreParameter.UnpackAlignment, oldAlignment);
    }

    public readonly void BindTexture(uint bindingIndex) => _gL.BindTextureUnit(bindingIndex, _handle);

    public void Dispose()
    {
        _gL.DeleteTexture(_handle);
    }
}

public partial struct GLTexture2D
{
    //public readonly void SetParameter(TextureParameterName parameterName, GLEnum value) 
    //    => _gL.TextureParameter(_handle, parameterName, (int)value);
}