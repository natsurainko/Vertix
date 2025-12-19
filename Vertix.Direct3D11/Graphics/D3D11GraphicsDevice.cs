using Silk.NET.Core.Contexts;
using Silk.NET.Core.Native;
using Silk.NET.Direct3D.Compilers;
using Silk.NET.Direct3D11;
using Silk.NET.DXGI;
using System;
using System.Runtime.CompilerServices;
using Vertix.Rendering;

namespace Vertix.Direct3D11.Graphics;

public partial class D3D11GraphicsDevice : IDisposable
{
    public DXGI DXGI { get; }

    public D3D11 D3D11 { get; }

    public D3DCompiler D3DCompiler { get; }

    public ComPtr<IDXGIFactory4> Factory;
    public ComPtr<IDXGISwapChain4> SwapChain;
    public ComPtr<ID3D11Device5> Device;
    public ComPtr<ID3D11DeviceContext4> DeviceContext;

    public ComPtr<ID3D11RenderTargetView> DefaultRenderTargetView;
    public ComPtr<ID3D11Texture2D> DefaultFrameBuffer;

    private IRenderTarget? _currentRenderTarget = null;

    internal unsafe D3D11GraphicsDevice(INativeWindowSource windowSource, bool dxvk = false)
    {
        DXGI = DXGI.GetApi(windowSource, dxvk);
        D3D11 = D3D11.GetApi(windowSource, dxvk);
        D3DCompiler = D3DCompiler.GetApi();

        Factory = DXGI.CreateDXGIFactory<IDXGIFactory4>();

        ComPtr<ID3D11Device> device = default;
        ComPtr<ID3D11DeviceContext> deviceContext = default;

        SilkMarshal.ThrowHResult
        (
            D3D11.CreateDevice
            (
                default(ComPtr<IDXGIAdapter>),
                D3DDriverType.Hardware,
                Software: default,
                (uint)CreateDeviceFlag.Debug,
                null,
                0,
                D3D11.SdkVersion,
                ref device,
                null,
                ref deviceContext
            )
        );

        SilkMarshal.ThrowHResult(device.QueryInterface(out Device));
        SilkMarshal.ThrowHResult(deviceContext.QueryInterface(out DeviceContext));

        device.Release();
        deviceContext.Release();

        SwapChainDesc1 swapChainDesc = new()
        {
            BufferCount = 2,
            Format = Format.FormatB8G8R8A8Unorm,
            BufferUsage = DXGI.UsageRenderTargetOutput,
            SwapEffect = SwapEffect.FlipSequential,
            SampleDesc = new SampleDesc(1, 0)
        };

        SilkMarshal.ThrowHResult
        (
            Factory.CreateSwapChainForHwnd
            (
                device,
                windowSource.Native!.DXHandle!.Value,
                in swapChainDesc,
                null,
                ref Unsafe.NullRef<IDXGIOutput>(),
                ref SwapChain
            )
        );

        DefaultFrameBuffer = SwapChain.GetBuffer<ID3D11Texture2D>(0);
        SilkMarshal.ThrowHResult(device.CreateRenderTargetView(DefaultFrameBuffer, null, ref DefaultRenderTargetView));
    }

    public void Dispose()
    {
        
    }
}
