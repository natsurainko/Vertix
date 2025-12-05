using Silk.NET.Maths;
using Silk.NET.OpenGL;
using Silk.NET.Windowing;
using Vertix.Graphics;
using Vertix.OpenGL.Graphics;
using Vertix.Windowing;

namespace Vertix.OpenGL.Windowing;

public partial class GLGameWindow : GameWindow
{
    protected readonly GL _gL;

    public override IGraphicsDevice Graphics { get; }

    public GLGameWindow(IWindow window, IGraphicsDevice graphicsDevice) : base(window)
    {
        Graphics = graphicsDevice;
        _gL = ((GLGraphicsDevice)graphicsDevice).GL;

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

public partial class GLGameWindow
{
    public static readonly WindowOptions DefaultGLWindowOptions;

    static GLGameWindow()
    {
        DefaultGLWindowOptions = WindowOptions.Default;
        DefaultGLWindowOptions.IsVisible = false;
        DefaultGLWindowOptions.Size = new Vector2D<int>(800, 600);
        DefaultGLWindowOptions.API = new GraphicsAPI
        (
            ContextAPI.OpenGL,
            ContextProfile.Core,
            ContextFlags.Default,
            new APIVersion(4, 6)
        );

        PlatformCreateWindowFuncs.Add(ContextAPI.OpenGL, Create);
        PlatformCreateCoreWindowFuncs.Add(ContextAPI.OpenGL, static options =>
        {
            IWindow coreWindow = Window.Create(options ?? DefaultGLWindowOptions);
            coreWindow.Initialize();
            return coreWindow;
        });
        PlatformCreateGraphicsDeviceFuncs.Add(ContextAPI.OpenGL, static window => new GLGraphicsDevice(window.CreateOpenGL()));
    }

    public static GameWindow Create(WindowOptions? windowOptions = null)
    {
        IWindow coreWindow = Window.Create(windowOptions ?? DefaultGLWindowOptions);
        coreWindow.Initialize();
        return new GLGameWindow(coreWindow, new GLGraphicsDevice(coreWindow.CreateOpenGL()));
    }
}