using GLRenderPipelineDemo;
using GLRenderPipelineDemo.Rendering;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Silk.NET.Windowing;
using Vertix.Content;
using Vertix.Host;
using Vertix.OpenGL.Windowing;

WindowOptions windowOptions = GLGameWindow.DefaultGLWindowOptions;
windowOptions.Size = new Silk.NET.Maths.Vector2D<int>(1280, 768);

new GameApplicationBuilder<GLRenderPipelineDemo.GameApplication>()
    .ConfigureWindow<MainWindow>(windowOptions)
    .ConfigureServices(services =>
    {
        services.AddSingleton<AssetImporter>();

        services.AddSingleton<GraphicsResources>();
        services.AddSingleton<RenderContext>();
        services.AddSingleton<RenderPipeline>();
    })
    .Build()
    .Run();