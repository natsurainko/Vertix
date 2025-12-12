using Silk.NET.OpenGL;
using System;
using Vertix.Graphics;
using Vertix.OpenGL.Graphics;

namespace Vertix.OpenGL.Extensions;

public static class IGraphicsBufferExtensions
{
    extension(IGraphicsBuffer graphicsBuffer)
    {
        public unsafe void Initialize<T>(int length, BufferStorageMask flags, ReadOnlySpan<T> data) where T : unmanaged
        {
            if (graphicsBuffer is not GLGraphicsBuffer) 
                throw new InvalidOperationException();

            graphicsBuffer.Initialize(length, (uint)flags, data);
        }
    }
}
