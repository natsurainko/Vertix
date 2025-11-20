using Silk.NET.Windowing;
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Vertix.Graphics;

[assembly: InternalsVisibleTo("Vertix.OpenGL")]
[assembly: InternalsVisibleTo("Vertix.Direct3D12")]

namespace Vertix.Windowing;

public abstract partial class GameWindow : IDisposable
{
    public IWindow CoreWindow { get; private set; }

    public abstract IGraphicsDevice Graphics { get; }

    internal GameWindow(IWindow window)
    {
        CoreWindow = window;
        CoreWindow.Update += OnUpdate;
        CoreWindow.Render += OnRender;
    }

    public abstract void Dispose();

    public abstract void Close();

    protected virtual void OnLoaded() { }

    protected virtual void OnUpdate(double deltaTime) { }

    protected virtual void OnRender(double delateTime) { }
}

public partial class GameWindow
{
    internal static Dictionary<ContextAPI, Func<WindowOptions?, GameWindow>> PlatformCreateWindowFuncs = [];
    internal static Dictionary<ContextAPI, Func<WindowOptions?, IWindow>> PlatformCreateCoreWindowFuncs = [];

    public static GameWindow Create(WindowOptions windowOptions)
    {
        if (PlatformCreateWindowFuncs.TryGetValue(windowOptions.API.API, out var func))
            return func(windowOptions);

        throw new InvalidOperationException("Target graphics api not register");
    }

    public static IWindow CreateCore(WindowOptions windowOptions)
    {
        if (PlatformCreateCoreWindowFuncs.TryGetValue(windowOptions.API.API, out var func))
            return func(windowOptions);

        throw new InvalidOperationException("Target graphics api not register");
    }
}