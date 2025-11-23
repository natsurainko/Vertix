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
