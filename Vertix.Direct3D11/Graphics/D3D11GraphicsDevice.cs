using Silk.NET.Core.Native;
using Silk.NET.Direct3D.Compilers;
using Silk.NET.Direct3D11;
using Silk.NET.DXGI;
using Silk.NET.Windowing;
using System;
using System.Runtime.CompilerServices;
using Vertix.Direct3D11.Rendering;
using Vertix.Graphics;

namespace Vertix.Direct3D11.Graphics;

public partial class D3D11GraphicsDevice : IDisposable
{
    private readonly IWindow _window;
    private D3D11Texture2D? _defaultFrameTexture;
    private D3D11Texture2D? _defaultDepthStencilTexture;
    private D3D11RenderTarget? _defaultRenderTarget;
    private D3D11RenderTarget? _currentRenderTarget = null;

    public ComPtr<IDXGIFactory4> Factory;
    public ComPtr<IDXGISwapChain4> SwapChain;
    public ComPtr<ID3D11Device5> Device;
    public ComPtr<ID3D11DeviceContext4> DeviceContext;

    public DXGI DXGI { get; }

    public D3D11 D3D11 { get; }

    public D3DCompiler D3DCompiler { get; }

    internal unsafe D3D11GraphicsDevice(IWindow window, bool dxvk = false)
    {
        _window = window;

        DXGI = DXGI.GetApi(window, dxvk);
        D3D11 = D3D11.GetApi(window, dxvk);
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
                window.Native!.DXHandle!.Value,
                in swapChainDesc,
                null,
                ref Unsafe.NullRef<IDXGIOutput>(),
                ref SwapChain
            )
        );

        CreateDefaultRenderTarget();
    }

    private void CreateDefaultRenderTarget()
    {
        _defaultRenderTarget = new D3D11RenderTarget(this, _window.Size.As<uint>());
        _defaultFrameTexture = new D3D11Texture2D(this, SwapChain.GetBuffer<ID3D11Texture2D>(0));

        (int, int) depthStencilFormat = 
        (
            _window.PreferredDepthBufferBits ?? 24,
            _window.PreferredStencilBufferBits ?? 8
        );

        _defaultDepthStencilTexture = new D3D11Texture2D(this);
        _defaultDepthStencilTexture.Initialize
        (
            _window.Size.As<uint>(),
            depthStencilFormat switch
            {
                (16, 0) => TextureFormat.Depth16,
                (24, 0) => TextureFormat.Depth24,
                (32, 0) => TextureFormat.Depth32,

                (24, 8) => TextureFormat.Depth24Stencil8,
                (32, 8) => TextureFormat.Depth32fStencil8,

                _ => TextureFormat.Depth24Stencil8
            }
        );

        _defaultRenderTarget.AttachTargetTexture(_defaultFrameTexture);
        _defaultRenderTarget.AttachTargetTexture(_defaultDepthStencilTexture, RenderTargetAttachment.DepthStencil);
        _defaultRenderTarget.Initialize();
    }

    public void Dispose()
    {
        _defaultFrameTexture?.Dispose();
        _defaultRenderTarget?.Dispose();

        DXGI.Dispose();
        D3D11.Dispose();
        D3DCompiler.Dispose();
    }
}
