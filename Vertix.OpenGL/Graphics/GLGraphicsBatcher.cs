using Silk.NET.OpenGL;
using System;
using System.Threading;
using Vertix.Graphics;
using PrimitiveType = Vertix.Graphics.PrimitiveType;

namespace Vertix.OpenGL.Graphics;

public class GLGraphicsBatcher<TInstance> : IGraphicsBatcher<TInstance> where TInstance : unmanaged
{
    private readonly int _capacity;

    private readonly GLGraphicsDevice _graphicsDevice;

    private readonly IVertexArray _vertexArray;
    private readonly IGraphicsBuffer _instanceBuffer;

    private readonly TInstance[] _instances;
    private int _currentIndex;

    public IGraphicsDevice GraphicsDevice => _graphicsDevice;

    public PrimitiveType PrimitiveType { get; set; } = PrimitiveType.Triangles;

    public uint VerticesOrIndicesCount { get; set; }

    public unsafe GLGraphicsBatcher(GLGraphicsDevice graphicsDevice, in IVertexArray vertexArray, ReadOnlySpan<VertexArrayProperty> properties, uint verticesOrIndicesCount, int capacity = 4096)
    {
        _graphicsDevice = graphicsDevice;
        _vertexArray = vertexArray;
        _capacity = capacity;
        _instances = new TInstance[capacity];

        VerticesOrIndicesCount = verticesOrIndicesCount;

        _instanceBuffer = graphicsDevice.CreateGraphicsBuffer();
        _instanceBuffer.Initialize(capacity, (uint)BufferStorageMask.DynamicStorageBit, ReadOnlySpan<TInstance>.Empty);
        _vertexArray.AttachInstanceBuffer<TInstance>(_instanceBuffer, properties);
    }

    public unsafe void DrawInstance(TInstance instanceData)
    {
        if (_currentIndex >= _capacity)
            Flush();

        _instances[_currentIndex] = instanceData;
        Interlocked.Increment(ref _currentIndex);
    }

    public void Flush()
    {
        if (_currentIndex == 0) return;
        _instanceBuffer.Fill(0, _currentIndex, _instances);

        if (_vertexArray.IndexBuffer != null)
            _graphicsDevice.DrawVertexElementsArrayInstanced(_vertexArray, PrimitiveType, VerticesOrIndicesCount, (uint)_currentIndex);
        else
            _graphicsDevice.DrawVertexArrayInstanced(_vertexArray, PrimitiveType, 0, VerticesOrIndicesCount, (uint)_currentIndex);

        Interlocked.Exchange(ref _currentIndex, 0);
    }

    public void Dispose()
    {
        _vertexArray.DetachBuffer(_instanceBuffer);
        _instanceBuffer.Dispose();
    }
}
