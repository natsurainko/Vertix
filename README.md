# Vertix
[![.NET Version](https://img.shields.io/badge/.NET-10.0-blue.svg)](https://dotnet.microsoft.com/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)  
A lightweight 3D game development framework for multiple graphics platforms, built on .NET 10 with Silk.NET bindings.  

### Features
+ [x] OpenGL 4.6 basic implementation
+ [x] Assimp-based 3D model importer

### In Progress
+ 🔄 Freetype font rendering
+ 🔄 ITexture interfaces

## Quick Start
Create your game application class by inheriting from `Vertix.Host.GameApplication` and create a window class that inherits from the graphics platform's base window class. For example, when using OpenGL, inherit from `GLGameWindow`.

```csharp
new GameApplicationBuilder<GLGameDemo.GameApplication>()
    .ConfigureWindow<DemoWindow>(GLGameWindow.DefaultGLWindowOptions)
    .ConfigureServices(services =>
    {
        services.AddSingleton<AssetImporter>();
    })
    .Build()
    .Run();
```

You can refer to the examples in the repository.

## Roadmap
- [ ] Complete GUI system
- [ ] Engine game object implementation
- [ ] BepuPhysics support
- [ ] D3D12 implementation
- [ ] Vulkan implementation

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
