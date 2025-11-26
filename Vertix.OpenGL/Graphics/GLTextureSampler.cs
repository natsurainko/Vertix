using Silk.NET.Maths;
using Silk.NET.OpenGL;
using System;
using Vertix.Extensions;
using Vertix.Graphics;
using Vertix.OpenGL.Helpers;

namespace Vertix.OpenGL.Graphics;

public struct GLTextureSampler : ITextureSampler
{
    private readonly GL _gL;
    private readonly uint _handle;

    public readonly uint Handle => _handle;

    public GLTextureSampler(GL gL)
    {
        _gL = gL;
        _handle = gL.CreateSampler();
    }

    public GLTextureSampler(GL gL, uint handle)
    {
        _gL = gL;
        _handle = handle;
    }

    public TextureFilter MinFilter 
    {
        readonly get => field;
        set
        {
            if (field != value)
            {
                field = value;
                _gL.SamplerParameter(_handle,  SamplerParameterI.MinFilter, value.ToGLFilter());
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
                _gL.SamplerParameter(_handle, SamplerParameterI.MagFilter, value.ToGLFilter());
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
                _gL.SamplerParameter(_handle, SamplerParameterI.WrapS, value.ToGLWarp());
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
                _gL.SamplerParameter(_handle, SamplerParameterI.WrapT, value.ToGLWarp());
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
                _gL.SamplerParameter(_handle, SamplerParameterI.WrapR, value.ToGLWarp());
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
                _gL.SamplerParameter(_handle, SamplerParameterF.LodBias, value);
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
                _gL.SamplerParameter(_handle, SamplerParameterF.MinLod, value);
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
                _gL.SamplerParameter(_handle, SamplerParameterF.MaxLod, value);
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
                _gL.SamplerParameter(_handle, SamplerParameterF.MaxAnisotropy, value);
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
                _gL.SamplerParameter(_handle, SamplerParameterF.BorderColor, floats);
            }
        }
    } = Vector4D<float>.Zero;

    public void BindSampler(uint bindingIndex) => _gL.BindSampler(bindingIndex, _handle);

    public void Dispose() 
    { 
        _gL.DeleteSampler(_handle);
    }
}
