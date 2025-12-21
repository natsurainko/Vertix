using Silk.NET.Core.Native;
using Silk.NET.Direct3D11;
using System;
using Vertix.Direct3D11.Helpers;
using Vertix.Graphics;

namespace Vertix.Direct3D11.Graphics;

public class D3D11GraphicsBuffer(D3D11GraphicsDevice graphicsDevice, GraphicsBufferUsage bufferUsage, GraphicsBufferMapAccess bufferMapAccess = GraphicsBufferMapAccess.None) : IGraphicsBuffer
{
    private readonly ComPtr<ID3D11Device5> _device = graphicsDevice.Device;
    private readonly ComPtr<ID3D11DeviceContext4> _context = graphicsDevice.DeviceContext;

    internal BufferDesc _bufferDesc;
    internal ComPtr<ID3D11Buffer> _buffer;
    internal readonly BindFlag _bindflags = ConvertUsageToBindFlags(bufferUsage);

    public bool Initialized { get; private set; }

    public GraphicsBufferUsage BufferUsage { get; } = bufferUsage;

    public GraphicsBufferMapAccess BufferMapAccess { get; } = bufferMapAccess;

    public unsafe void Initialize<T>(int length, bool fillable, ReadOnlySpan<T> data = default)
        where T : unmanaged
    {
        SubresourceData initialData = default;
        _bufferDesc = new()
        {
            ByteWidth = (uint)(length * sizeof(T)),
            BindFlags = (uint)_bindflags,
            Usage = Usage.Default,
            CPUAccessFlags = (uint)CpuAccessFlag.None
        };

        if (!fillable && BufferMapAccess == GraphicsBufferMapAccess.None)
            _bufferDesc.Usage = Usage.Immutable;

        if (BufferMapAccess != GraphicsBufferMapAccess.None)
        {
            if (fillable) throw new InvalidOperationException();

            _bufferDesc.Usage = Usage.Dynamic;
            _bufferDesc.CPUAccessFlags = (uint)ConvertAccessToCpuAccessFlags(BufferMapAccess);
        }

        if (data.Length > 0)
        {
            fixed (void* dataPtr = data)
                initialData.PSysMem = dataPtr;
        }

        SilkMarshal.ThrowHResult(_device.CreateBuffer(in _bufferDesc, in initialData, ref _buffer));
        Initialized = true;
    }

    public void BindAsUniform(uint bindingIndex)
    {
        throw new NotImplementedException();
    }

    public unsafe void Fill<T>(nint offset, int length, ReadOnlySpan<T> data) where T : unmanaged
    {
        if (!Initialized || ((_bufferDesc.Usage & Usage.Dynamic) != Usage.Dynamic))
            throw new InvalidOperationException("Buffer is not initialized with dynamic usage.");

        fixed (void* dataPtr = data)
        {
            Box box = new()
            {
                Left = (uint)(offset * sizeof(T)),
                Right = (uint)((offset + length) * sizeof(T)),
                Top = 0,
                Bottom = 1,
                Front = 0,
                Back = 1
            };

            _context.UpdateSubresource
            (
                _buffer,
                0,
                &box,
                dataPtr,
                0,
                0
            );
        }
    }

    public void Dispose() => _buffer.Dispose();

    private static BindFlag ConvertUsageToBindFlags(GraphicsBufferUsage usages)
    {
        BindFlag bindFlags = 0;

        foreach (GraphicsBufferUsage usage in Enum.GetValues<GraphicsBufferUsage>())
            if ((usages & usage) == usage)
                bindFlags |= usage.ToBindFlag();

        return bindFlags;
    }

    private static CpuAccessFlag ConvertAccessToCpuAccessFlags(GraphicsBufferMapAccess access)
    {
        CpuAccessFlag cpuAccessFlags = 0;

        foreach (GraphicsBufferMapAccess bufferAccess in Enum.GetValues<GraphicsBufferMapAccess>())
            if ((access & bufferAccess) == bufferAccess)
                cpuAccessFlags |= bufferAccess.ToCpuAccessFlag();

        return cpuAccessFlags;
    }
}
