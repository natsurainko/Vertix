using System;

namespace Vertix.Graphics;

public interface IGraphicsBatcher<TInstance> : IDisposable where TInstance : unmanaged
{
    IGraphicsDevice GraphicsDevice { get; }

    PrimitiveType PrimitiveType { get; set; }

    uint VerticesOrIndicesCount { get; set; }

    void DrawInstance(TInstance instanceData);

    void Flush();
}