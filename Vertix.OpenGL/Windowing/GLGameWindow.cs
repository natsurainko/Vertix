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

    public GLGameWindow(IWindow window) : base(window)
    {
        window.Initialize();
        _gL = window.CreateOpenGL();
        Graphics = new GLGraphicsDevice(_gL);
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
        PlatformCreateCoreWindowFuncs.Add(ContextAPI.OpenGL, options => Window.Create(options ?? DefaultGLWindowOptions));
    }

    public static GameWindow Create(WindowOptions? windowOptions = null)
    {
        IWindow coreWindow = Window.Create(windowOptions ?? DefaultGLWindowOptions);
        return new GLGameWindow(coreWindow);
    }
}