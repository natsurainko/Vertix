using Silk.NET.Direct3D11;
using Silk.NET.DXGI;
using System;
using Vertix.Graphics;

namespace Vertix.Direct3D11.Helpers;

public static class EnumHelper
{
    extension(TextureFormat format)
    {
        public Format ToDXGIFormat() => format switch
        {
            TextureFormat.R8 => Format.FormatR8Unorm,
            TextureFormat.R16 => Format.FormatR16Unorm,

            TextureFormat.RG8 => Format.FormatR8G8Unorm,
            TextureFormat.RG16 => Format.FormatR16G16Unorm,

            TextureFormat.Rgba8 => Format.FormatR8G8B8A8Unorm,

            TextureFormat.Bgra8 => Format.FormatB8G8R8A8Unorm,

            TextureFormat.Srgb8 => Format.FormatB8G8R8X8UnormSrgb,
            TextureFormat.Srgb8Alpha8 => Format.FormatB8G8R8A8UnormSrgb,

            TextureFormat.R16f => Format.FormatR16Float,
            TextureFormat.R32f => Format.FormatR32Float,

            TextureFormat.RG16f => Format.FormatR16G16Float,
            TextureFormat.RG32f => Format.FormatR32G32Float,

            TextureFormat.Rgba16f => Format.FormatR16G16B16A16Float,
            TextureFormat.Rgba32f => Format.FormatR32G32B32A32Float,

            TextureFormat.Depth16 => Format.FormatD16Unorm,
            TextureFormat.Depth32f => Format.FormatD32Float,

            TextureFormat.Depth24Stencil8 => Format.FormatD24UnormS8Uint,
            TextureFormat.Depth32fStencil8 => Format.FormatD32FloatS8X24Uint,

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
            TextureFormat.R8 => 1,
            TextureFormat.R16 => 2,
            TextureFormat.RG8 => 2,
            TextureFormat.RG16 => 4,
            TextureFormat.Rgba8 => 4,
            TextureFormat.Bgra8 => 4,
            TextureFormat.Srgb8 => 4,
            TextureFormat.Srgb8Alpha8 => 4,
            TextureFormat.R16f => 2,
            TextureFormat.R32f => 4,
            TextureFormat.RG16f => 4,
            TextureFormat.RG32f => 8,
            TextureFormat.Rgba16f => 8,
            TextureFormat.Rgba32f => 16,
            TextureFormat.Depth16 => 2,
            TextureFormat.Depth32f => 4,
            TextureFormat.Depth24Stencil8 => 4,
            TextureFormat.Depth32fStencil8 => 8,
            _ => throw new NotSupportedException($"Unsupported texture format: {format}"),
        };
    }

    extension(Format dxgiFormat)
    {
        public TextureFormat ToTextureFormat() => dxgiFormat switch
        {
            Format.FormatR8Unorm => TextureFormat.R8,
            Format.FormatR16Unorm => TextureFormat.R16,

            Format.FormatR8G8Unorm => TextureFormat.RG8,
            Format.FormatR16G16Unorm => TextureFormat.RG16,

            Format.FormatR8G8B8A8Unorm => TextureFormat.Rgba8,
            Format.FormatB8G8R8A8Unorm => TextureFormat.Bgra8,

            Format.FormatB8G8R8X8UnormSrgb => TextureFormat.Srgb8,
            Format.FormatB8G8R8A8UnormSrgb => TextureFormat.Srgb8Alpha8,

            Format.FormatR16Float => TextureFormat.R16f,
            Format.FormatR32Float => TextureFormat.R32f,

            Format.FormatR16G16Float => TextureFormat.RG16f,
            Format.FormatR32G32Float => TextureFormat.RG32f,

            Format.FormatR16G16B16A16Float => TextureFormat.Rgba16f,
            Format.FormatR32G32B32A32Float => TextureFormat.Rgba32f,

            Format.FormatD16Unorm => TextureFormat.Depth16,
            Format.FormatD32Float => TextureFormat.Depth32f,

            Format.FormatD24UnormS8Uint => TextureFormat.Depth24Stencil8,
            Format.FormatD32FloatS8X24Uint => TextureFormat.Depth32fStencil8,

            _ => throw new NotSupportedException($"Unsupported DXGI format: {dxgiFormat}")
        };
    }

    extension(GraphicsBufferUsage bufferUsage)
    {
        public BindFlag ToBindFlag() => bufferUsage switch
        {
            GraphicsBufferUsage.VertexBuffer => BindFlag.VertexBuffer,
            GraphicsBufferUsage.IndexBuffer => BindFlag.IndexBuffer,
            GraphicsBufferUsage.UniformBuffer => BindFlag.ConstantBuffer,
            _ => BindFlag.None,
        };
    }

    extension(GraphicsBufferMapAccess bufferAccess)
    {
        public CpuAccessFlag ToCpuAccessFlag() => bufferAccess switch
        {
            GraphicsBufferMapAccess.Read => CpuAccessFlag.Read,
            GraphicsBufferMapAccess.Write => CpuAccessFlag.Write,
            _ => CpuAccessFlag.None,
        };
    }
}
