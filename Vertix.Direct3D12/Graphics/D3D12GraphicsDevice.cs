using Silk.NET.Core.Native;
using Silk.NET.Direct3D12;
using Silk.NET.DXGI;
using System;

namespace Vertix.Direct3D12.Windowing;

public partial class D3D12GraphicsDevice : IDisposable
{
    protected readonly DXGI _dXGI;
    protected readonly D3D12 _d3D12;

    private ComPtr<ID3D12Device> _device;
    private ComPtr<ID3D12CommandQueue> _commandQueue;
    private ComPtr<ID3D12GraphicsCommandList> _commandList;
    private ComPtr<ID3D12CommandAllocator> _commandAllocator;
    private ComPtr<IDXGISwapChain3> _swapChain;
    private ComPtr<ID3D12Resource>[] _renderTargets;
    private ComPtr<ID3D12DescriptorHeap> _rtvHeap;
    private ComPtr<ID3D12Resource> _depthStencilBuffer;
    private ComPtr<ID3D12DescriptorHeap> _dsvHeap;
    private uint _rtvDescriptorSize;
    private int _frameIndex;
    private const int FrameCount = 2;

    internal D3D12GraphicsDevice(DXGI dXGI, D3D12 d3D12)
    {
        _dXGI = dXGI;
        _d3D12 = d3D12;
    }

    public void Dispose()
    {
        _dXGI.Dispose();
        _d3D12.Dispose();
    }
}