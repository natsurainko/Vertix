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
