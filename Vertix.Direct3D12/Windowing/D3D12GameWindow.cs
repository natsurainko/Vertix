using Silk.NET.Direct3D12;
using Silk.NET.DXGI;
using Silk.NET.Maths;
using Silk.NET.Windowing;
using System;
using Vertix.Graphics;
using Vertix.Windowing;

namespace Vertix.Direct3D12.Windowing;

public partial class D3D12GameWindow : GameWindow
{
    protected readonly DXGI _dXGI;
    protected readonly D3D12 _d3D12;
    protected readonly IntPtr? _hwnd;

    public override IGraphicsDevice Graphics { get; }

    public D3D12GameWindow(IWindow window) : base(window)
    {
        window.Initialize();

        _d3D12 = D3D12.GetApi();
        _dXGI = DXGI.GetApi(window);
        _hwnd = window.Native?.Win32?.Hwnd;

        if (_hwnd == null)
            throw new PlatformNotSupportedException("D3D12 currently only supports Windows with Win32 windows!");

        Graphics = new D3D12GraphicsDevice(_dXGI, _d3D12);
        window.Center();
        window.IsVisible = true;
        OnLoaded();
    }

    public override void Close() => CoreWindow.Close();

    public override void Dispose()
    {
        CoreWindow.Dispose();
        Graphics.Dispose();
    }
}

public partial class D3D12GameWindow
{
    public static readonly WindowOptions DefaultGLWindowOptions;

    static D3D12GameWindow()
    {
        DefaultGLWindowOptions = WindowOptions.Default;
        DefaultGLWindowOptions.IsVisible = false;
        DefaultGLWindowOptions.Size = new Vector2D<int>(800, 600);
        DefaultGLWindowOptions.API = GraphicsAPI.None;

        PlatformCreateWindowFuncs.Add(ContextAPI.None, Create);
        PlatformCreateCoreWindowFuncs.Add(ContextAPI.None, options => Window.Create(options ?? DefaultGLWindowOptions));
    }

    public static GameWindow Create(WindowOptions? windowOptions = null)
    {
        IWindow nativeWindow = Window.Create(windowOptions ?? DefaultGLWindowOptions);
        return new D3D12GameWindow(nativeWindow);
    }
}