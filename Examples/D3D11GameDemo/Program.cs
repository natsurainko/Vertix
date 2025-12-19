using D3D11GameDemo;
using Microsoft.Extensions.Hosting;
using Vertix.Direct3D11.Windowing;
using Vertix.Host;

new GameApplicationBuilder<D3D11GameDemo.GameApplication>()
    .ConfigureWindow<MainWindow>(D3D11GameWindow.DefaultD3D11WindowOptions)
    //.ConfigureServices(services =>
    //{
    //})
    .Build()
    .Run();