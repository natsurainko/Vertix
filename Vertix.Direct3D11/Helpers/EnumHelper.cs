using Silk.NET.DXGI;
using System;

namespace Vertix.Direct3D11.Helpers;

public static class EnumHelper
{
    extension(Vertix.Graphics.TextureFormat format)
    {
        public Format ToDXGIFormat() => format switch
        {
            Vertix.Graphics.TextureFormat.R8 => Format.FormatR8Unorm,
            Vertix.Graphics.TextureFormat.R16 => Format.FormatR16Unorm,

            Vertix.Graphics.TextureFormat.RG8 => Format.FormatR8G8Unorm,
            Vertix.Graphics.TextureFormat.RG16 => Format.FormatR16G16Unorm,

            Vertix.Graphics.TextureFormat.Rgba8 => Format.FormatR8G8B8A8Unorm,

            Vertix.Graphics.TextureFormat.Bgra8 => Format.FormatB8G8R8A8Unorm,

            Vertix.Graphics.TextureFormat.Srgb8 => Format.FormatB8G8R8X8UnormSrgb,
            Vertix.Graphics.TextureFormat.Srgb8Alpha8 => Format.FormatB8G8R8A8UnormSrgb,

            Vertix.Graphics.TextureFormat.R16f => Format.FormatR16Float,
            Vertix.Graphics.TextureFormat.R32f => Format.FormatR32Float,

            Vertix.Graphics.TextureFormat.RG16f => Format.FormatR16G16Float,
            Vertix.Graphics.TextureFormat.RG32f => Format.FormatR32G32Float,

            Vertix.Graphics.TextureFormat.Rgba16f => Format.FormatR16G16B16A16Float,
            Vertix.Graphics.TextureFormat.Rgba32f => Format.FormatR32G32B32A32Float,

            Vertix.Graphics.TextureFormat.Depth16 => Format.FormatD16Unorm,
            Vertix.Graphics.TextureFormat.Depth32f => Format.FormatD32Float,

            Vertix.Graphics.TextureFormat.Depth24Stencil8 => Format.FormatD24UnormS8Uint,
            Vertix.Graphics.TextureFormat.Depth32fStencil8 => Format.FormatD32FloatS8X24Uint,

            //Vertix.Graphics.TextureFormat.Rgb8
            //Vertix.Graphics.TextureFormat.Rgba16

            //Vertix.Graphics.TextureFormat.Bgr8
            //Vertix.Graphics.TextureFormat.Bgra16

            //Vertix.Graphics.TextureFormat.Rgb16f
            //Vertix.Graphics.TextureFormat.Rgb32f

            //Vertix.Graphics.TextureFormat.Depth24
            //Vertix.Graphics.TextureFormat.Depth32

            _ => throw new NotSupportedException($"Unsupported texture format: {format}"),
        };

        public uint GetBytesPerPixel() => format switch
        {
            Vertix.Graphics.TextureFormat.R8 => 1,
            Vertix.Graphics.TextureFormat.R16 => 2,
            Vertix.Graphics.TextureFormat.RG8 => 2,
            Vertix.Graphics.TextureFormat.RG16 => 4,
            Vertix.Graphics.TextureFormat.Rgba8 => 4,
            Vertix.Graphics.TextureFormat.Bgra8 => 4,
            Vertix.Graphics.TextureFormat.Srgb8 => 4,
            Vertix.Graphics.TextureFormat.Srgb8Alpha8 => 4,
            Vertix.Graphics.TextureFormat.R16f => 2,
            Vertix.Graphics.TextureFormat.R32f => 4,
            Vertix.Graphics.TextureFormat.RG16f => 4,
            Vertix.Graphics.TextureFormat.RG32f => 8,
            Vertix.Graphics.TextureFormat.Rgba16f => 8,
            Vertix.Graphics.TextureFormat.Rgba32f => 16,
            Vertix.Graphics.TextureFormat.Depth16 => 2,
            Vertix.Graphics.TextureFormat.Depth32f => 4,
            Vertix.Graphics.TextureFormat.Depth24Stencil8 => 4,
            Vertix.Graphics.TextureFormat.Depth32fStencil8 => 8,
            _ => throw new NotSupportedException($"Unsupported texture format: {format}"),
        };
    }
}
