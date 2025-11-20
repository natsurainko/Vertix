using GLGameDemo;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Silk.NET.Windowing;
using Vertix.Content;
using Vertix.Host;
using Vertix.OpenGL.Windowing;

new GameApplicationBuilder<GLGameDemo.GameApplication>()
    .ConfigureWindow<MainGameWindow>(GLGameWindow.DefaultGLWindowOptions)
    .ConfigureServices(services =>
    {
        services.AddSingleton<AssetImporter>();
    })
    .Build()
    .Run();