using Silk.NET.OpenGL;
using System;

namespace Vertix.OpenGL.Helpers;

public static class EnumHelper
{
    extension(Vertix.Graphics.PrimitiveType primitiveType)
    {
        public PrimitiveType ToGLEnum() => primitiveType switch
        {
            Vertix.Graphics.PrimitiveType.Points => PrimitiveType.Points,
            Vertix.Graphics.PrimitiveType.Lines => PrimitiveType.Lines,
            Vertix.Graphics.PrimitiveType.Triangles => PrimitiveType.Triangles,
            Vertix.Graphics.PrimitiveType.Quads => PrimitiveType.Quads,
            _ => throw new NotSupportedException(),
        };
    }

    extension(Vertix.Graphics.TextureFilter textureFilter)
    {
        public int ToGLFilter() => textureFilter switch
        {
            Vertix.Graphics.TextureFilter.Nearest => 9728,
            Vertix.Graphics.TextureFilter.Linear => 9729,
            Vertix.Graphics.TextureFilter.LinearMipmapLinear => (int)TextureMinFilter.LinearMipmapLinear,
            Vertix.Graphics.TextureFilter.LinearMipmapNearest => (int)TextureMinFilter.LinearMipmapNearest,
            Vertix.Graphics.TextureFilter.NearestMipmapLinear => (int)TextureMinFilter.NearestMipmapLinear,
            _ => 9728,
        };
    }

    extension(Vertix.Graphics.TextureFormat internalFormat)
    {
        public SizedInternalFormat ToGLSizedInternalFormat() => internalFormat switch
        {
            Vertix.Graphics.TextureFormat.R8 => SizedInternalFormat.R8,
            Vertix.Graphics.TextureFormat.R16 => SizedInternalFormat.R16,

            Vertix.Graphics.TextureFormat.RG8 => SizedInternalFormat.RG8,
            Vertix.Graphics.TextureFormat.RG16 => SizedInternalFormat.RG16,

            Vertix.Graphics.TextureFormat.Rgb8 => SizedInternalFormat.Rgb8,
            Vertix.Graphics.TextureFormat.Rgba8 => SizedInternalFormat.Rgba8,
            Vertix.Graphics.TextureFormat.Rgba16 => SizedInternalFormat.Rgba16,

            Vertix.Graphics.TextureFormat.Bgr8 => SizedInternalFormat.Rgb8,
            Vertix.Graphics.TextureFormat.Bgra8 => SizedInternalFormat.Rgba8,
            Vertix.Graphics.TextureFormat.Bgra16 => SizedInternalFormat.Rgba16,

            Vertix.Graphics.TextureFormat.Srgb8 => SizedInternalFormat.Srgb8,
            Vertix.Graphics.TextureFormat.Srgb8Alpha8 => SizedInternalFormat.Srgb8Alpha8,

            Vertix.Graphics.TextureFormat.R16f => SizedInternalFormat.R16f,
            Vertix.Graphics.TextureFormat.R32f => SizedInternalFormat.R32f,

            Vertix.Graphics.TextureFormat.RG16f => SizedInternalFormat.RG16f,
            Vertix.Graphics.TextureFormat.RG32f => SizedInternalFormat.RG32f,

            Vertix.Graphics.TextureFormat.Rgb16f => SizedInternalFormat.Rgb16f,
            Vertix.Graphics.TextureFormat.Rgb32f => SizedInternalFormat.Rgb32f,

            Vertix.Graphics.TextureFormat.Rgba16f => SizedInternalFormat.Rgba16f,
            Vertix.Graphics.TextureFormat.Rgba32f => SizedInternalFormat.Rgba32f,

            Vertix.Graphics.TextureFormat.Depth24Stencil8 => SizedInternalFormat.Depth24Stencil8,
            Vertix.Graphics.TextureFormat.Depth32fStencil8 => SizedInternalFormat.Depth32fStencil8,

            _ => SizedInternalFormat.Rgba8,
        };

        public void ToPixelFormatAndType(out PixelFormat pixelFormat, out PixelType pixelType)
        {
            switch (internalFormat)
            {
                case Vertix.Graphics.TextureFormat.R8:
                    pixelFormat = PixelFormat.Red;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.R16:
                    pixelFormat = PixelFormat.Red;
                    pixelType = PixelType.UnsignedShort;
                    break;

                case Vertix.Graphics.TextureFormat.RG8:
                    pixelFormat = PixelFormat.RG;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.RG16:
                    pixelFormat = PixelFormat.RG;
                    pixelType = PixelType.UnsignedShort;
                    break;

                case Vertix.Graphics.TextureFormat.Rgb8:
                    pixelFormat = PixelFormat.Rgb;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.Rgba8:
                    pixelFormat = PixelFormat.Rgba;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.Rgba16:
                    pixelFormat = PixelFormat.Rgba;
                    pixelType = PixelType.UnsignedShort;
                    break;

                case Vertix.Graphics.TextureFormat.Bgr8:
                    pixelFormat = PixelFormat.Bgr;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.Bgra8:
                    pixelFormat = PixelFormat.Bgra;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.Bgra16:
                    pixelFormat = PixelFormat.Bgra;
                    pixelType = PixelType.UnsignedShort;
                    break;

                case Vertix.Graphics.TextureFormat.Srgb8:
                    pixelFormat = PixelFormat.Rgb;
                    pixelType = PixelType.UnsignedByte;
                    break;
                case Vertix.Graphics.TextureFormat.Srgb8Alpha8:
                    pixelFormat = PixelFormat.Rgba;
                    pixelType = PixelType.UnsignedByte;
                    break;

                case Vertix.Graphics.TextureFormat.R16f:
                    pixelFormat = PixelFormat.Red;
                    pixelType = PixelType.HalfFloat;
                    break;
                case Vertix.Graphics.TextureFormat.R32f:
                    pixelFormat = PixelFormat.Red;
                    pixelType = PixelType.Float;
                    break;

                case Vertix.Graphics.TextureFormat.RG16f:
                    pixelFormat = PixelFormat.RG;
                    pixelType = PixelType.HalfFloat;
                    break;
                case Vertix.Graphics.TextureFormat.RG32f:
                    pixelFormat = PixelFormat.RG;
                    pixelType = PixelType.Float;
                    break;

                case Vertix.Graphics.TextureFormat.Rgb16f:
                    pixelFormat = PixelFormat.Rgb;
                    pixelType = PixelType.HalfFloat;
                    break;
                case Vertix.Graphics.TextureFormat.Rgb32f:
                    pixelFormat = PixelFormat.Rgb;
                    pixelType = PixelType.Float;
                    break;

                case Vertix.Graphics.TextureFormat.Rgba16f:
                    pixelFormat = PixelFormat.Rgba;
                    pixelType = PixelType.HalfFloat;
                    break;
                case Vertix.Graphics.TextureFormat.Rgba32f:
                    pixelFormat = PixelFormat.Rgba;
                    pixelType = PixelType.Float;
                    break;

                case Vertix.Graphics.TextureFormat.Depth24Stencil8:
                    pixelFormat = PixelFormat.DepthStencil;
                    pixelType = PixelType.UnsignedInt248;
                    break;
                case Vertix.Graphics.TextureFormat.Depth32fStencil8:
                    pixelFormat = PixelFormat.DepthStencil;
                    pixelType = PixelType.Float32UnsignedInt248Rev;
                    break;

                default:
                    pixelFormat = PixelFormat.Rgba;
                    pixelType = PixelType.UnsignedByte;
                    break;
            }
        }
    }

    extension(Vertix.Graphics.TextureAddressMode addressMode)
    {
        public int ToGLWarp() => addressMode switch
        {
            Vertix.Graphics.TextureAddressMode.Repeat => (int)TextureWrapMode.Repeat,
            Vertix.Graphics.TextureAddressMode.Mirror => (int)TextureWrapMode.MirroredRepeat,
            Vertix.Graphics.TextureAddressMode.ClampToEdge => (int)TextureWrapMode.ClampToEdge,
            Vertix.Graphics.TextureAddressMode.ClampToBorder => (int)TextureWrapMode.ClampToBorder,
            _ => (int)TextureWrapMode.Repeat,
        };
    }

    public static VertexAttribType? GetVertexAttribType(string typeName)
    {
        return typeName switch
        {
            "System.Single" => VertexAttribType.Float,
            "System.Double" => VertexAttribType.Double,
            "System.Byte" => VertexAttribType.Byte,
            "System.SByte" => VertexAttribType.Byte,
            "System.UInt16" => VertexAttribType.UnsignedShort,
            "System.Int16" => VertexAttribType.Short,
            "System.UInt32" => VertexAttribType.UnsignedInt,
            "System.Int32" => VertexAttribType.Int,
            _ => null,
        };
    }


}
