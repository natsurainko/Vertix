using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;

namespace Vertix.OpenGL.Graphics;

public struct GLTextureSampler(GL gL, GLTexture2D texture2D) : ITextureSampler
{
    private readonly uint _textureHandle = texture2D.Handle;

    public TextureFilter MinFilter 
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureMinFilter, value.ToGLFilter());
            }
        }
    } = TextureFilter.NearestMipmapLinear;
    public TextureFilter MagFilter
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureMagFilter, value.ToGLFilter());
            }
        }
    } = TextureFilter.Linear;

    public TextureAddressMode AddressU
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureWrapS, value.ToGLWarp());
            }
        }
    } = TextureAddressMode.Repeat;
    public TextureAddressMode AddressV
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureWrapT, value.ToGLWarp());
            }
        }
    } = TextureAddressMode.Repeat;
    public TextureAddressMode AddressW
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureWrapR, value.ToGLWarp());
            }
        }
    } = TextureAddressMode.Repeat;

    public float MipLODBias
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureLodBias, value);
            }
        }
    } = 0.0f;
    public float MinLOD
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureMinLod, value);
            }
        }
    } = -1000.0f;
    public float MaxLOD
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureMaxLod, value);
            }
        }
    } = 1000.0f;

    public int MaxAnisotropy
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureMaxAnisotropy, value);
            }
        }
    } = 1;

    //ComparisonFunction ComparisonFunc { get; set; }
    //bool ComparisonEnabled { get; set; }

    public Vector4D<float> BorderColor
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                Span<float> floats = [field.X, field.Y, field.Z, field.W];
                gL.TextureParameter(_textureHandle, TextureParameterName.TextureBorderColor, floats);
            }
        }
    } = Vector4D<float>.Zero;

    public void Dispose() { }
}
