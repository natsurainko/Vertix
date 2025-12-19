using Silk.NET.Core.Native;
using Silk.NET.DXGI;
using Silk.NET.Maths;
using Silk.NET.Windowing;
using Vertix.Direct3D11.Extensions;
using Vertix.Direct3D11.Graphics;
using Vertix.Graphics;
using Vertix.Windowing;

namespace Vertix.Direct3D11.Windowing;

public partial class D3D11GameWindow : GameWindow
{
    internal ComPtr<IDXGISwapChain4> _swapChain;

    public override IGraphicsDevice Graphics { get; }

    public D3D11GameWindow(IWindow window, IGraphicsDevice graphicsDevice) : base(window)
    {
        Graphics = graphicsDevice;
        D3D11GraphicsDevice d3D11Graphics = (D3D11GraphicsDevice)graphicsDevice;
        _swapChain = d3D11Graphics.SwapChain;

        window.Center();
        window.IsVisible = true;
        OnLoaded();
    }

    public override void Close()
    {

    }

    public override void Dispose()
    {

    }

    internal override void OnInternalRender(double delateTime)
    {
        OnRender(delateTime);

        if (CoreWindow.ShouldSwapAutomatically)
            this.SwapBuffers(CoreWindow.VSync);
    }
}

public partial class D3D11GameWindow
{
    public static readonly WindowOptions DefaultD3D11WindowOptions;

    static D3D11GameWindow()
    {
        DefaultD3D11WindowOptions = WindowOptions.Default;
        DefaultD3D11WindowOptions.IsVisible = false;
        DefaultD3D11WindowOptions.Size = new Vector2D<int>(800, 600);
        DefaultD3D11WindowOptions.API = GraphicsAPI.None;

        PlatformCreateWindowFuncs.Add(ContextAPI.None, Create);
        PlatformCreateCoreWindowFuncs.Add(ContextAPI.None, static options =>
        {
            IWindow coreWindow = Window.Create(options ?? DefaultD3D11WindowOptions);
            coreWindow.Initialize();
            return coreWindow;
        });
        PlatformCreateGraphicsDeviceFuncs.Add(ContextAPI.None, static window => new D3D11GraphicsDevice(window));
    }

    public static GameWindow Create(WindowOptions? windowOptions = null)
    {
        IWindow coreWindow = Window.Create(windowOptions ?? DefaultD3D11WindowOptions);
        coreWindow.Initialize();
        return new D3D11GameWindow(coreWindow, new D3D11GraphicsDevice(coreWindow));
    }
}