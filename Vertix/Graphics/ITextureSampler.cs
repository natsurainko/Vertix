using Silk.NET.Maths;
using System;

namespace Vertix.Graphics;

public interface ITextureSampler : IDisposable
{
    TextureFilter MinFilter { get; set; }
    TextureFilter MagFilter { get; set; }

    TextureAddressMode AddressU { get; set; }
    TextureAddressMode AddressV { get; set; }
    TextureAddressMode AddressW { get; set; }

    float MipLODBias { get; set; }
    float MinLOD { get; set; }
    float MaxLOD { get; set; }

    int MaxAnisotropy { get; set; }

    //ComparisonFunction ComparisonFunc { get; set; }
    //bool ComparisonEnabled { get; set; }

    Vector4D<float> BorderColor { get; set; }

    void BindSampler(uint bindingIndex);
}

public enum TextureFilter
{
    Nearest,
    Linear,

    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear,
}

public enum TextureAddressMode
{
    Repeat,
    Mirror,
    ClampToEdge,
    ClampToBorder,
    //MirrorOnce
}