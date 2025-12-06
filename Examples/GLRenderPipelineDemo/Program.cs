using GLRenderPipelineDemo;
using GLRenderPipelineDemo.Rendering;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Silk.NET.Windowing;
using Vertix.Content;
using Vertix.Host;
using Vertix.OpenGL.Windowing;

new GameApplicationBuilder<GLRenderPipelineDemo.GameApplication>()
    .ConfigureWindow<MainWindow>(GLGameWindow.DefaultGLWindowOptions)
    .ConfigureServices(services =>
    {
        services.AddSingleton<AssetImporter>();

        services.AddSingleton<GraphicsResources>();
        services.AddSingleton<RenderContext>();
        services.AddSingleton<RenderPipeline>();
    })
    .Build()
    .Run();