using Silk.NET.OpenGL;
using System;
using System.Threading;
using Vertix.Graphics;
using PrimitiveType = Vertix.Graphics.PrimitiveType;

namespace Vertix.OpenGL.Graphics;

public class GLGraphicsBatcher<TInstance> : IDisposable where TInstance : unmanaged
{
    private readonly int _capacity;

    private readonly GLGraphicsDevice _graphicsDevice;
    private readonly GLVertexArray _vertexArray;
    private readonly IGraphicsBuffer _instanceBuffer;

    private int _currentInstanceCount;

    public PrimitiveType PrimitiveType { get; set; } = PrimitiveType.Triangles;

    public uint VerticesOrIndicesCount { get; set; }

    public unsafe GLGraphicsBatcher(GLGraphicsDevice graphicsDevice, ref GLVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 4096)
    {
        _graphicsDevice = graphicsDevice;
        _vertexArray = vertexArray;
        _capacity = capacity;

        VerticesOrIndicesCount = verticesOrIndicesCount;

        _instanceBuffer = graphicsDevice.CreateGraphicsBuffer();
        _instanceBuffer.Initialize(capacity, (uint)BufferStorageMask.DynamicStorageBit, ReadOnlySpan<TInstance>.Empty);
        _vertexArray.AttachInstanceBuffer<TInstance>(_instanceBuffer, properties);
    }

    public unsafe void DrawInstance(TInstance instanceData)
    {
        if (_currentInstanceCount >= _capacity)
            Flush();

        IntPtr offset = _currentInstanceCount * sizeof(TInstance);
        _instanceBuffer.Fill(offset, 1, [instanceData]);

        Interlocked.Increment(ref _currentInstanceCount);
    }

    public void Flush()
    {
        if (_currentInstanceCount == 0)
            return;

        _vertexArray.Bind();

        if (_vertexArray.IndexBuffer != null)
            _graphicsDevice.DrawVertexElementsArrayInstanced(_vertexArray, PrimitiveType, VerticesOrIndicesCount, (uint)_currentInstanceCount);
        else
            _graphicsDevice.DrawVertexArrayInstanced(_vertexArray, PrimitiveType, 0, VerticesOrIndicesCount, (uint)_currentInstanceCount);

        Interlocked.Exchange(ref _currentInstanceCount, 0);
    }

    public void Dispose()
    {
        _vertexArray.DetachBuffer(_instanceBuffer);
        _instanceBuffer.Dispose();
    }
}
