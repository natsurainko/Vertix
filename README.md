# Vertix
[![.NET Version](https://img.shields.io/badge/.NET-10.0-blue.svg)](https://dotnet.microsoft.com/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)  
A lightweight 3D game development framework for multiple graphics platforms, built on .NET 10 with Silk.NET bindings.  

### Features
+ [x] OpenGL 4.6 basic implementation  
+ [x] Assimp-based 3D model importer  
+ [x] Basic texture sampler interfaces  
+ [x] Basic font rendering based on FreeType  
+ [x] RenderTarget and MRT (multi-RenderTarget) support  
+ [x] RenderPipeline abstraction implementation  
+ [x] Basic camera, scene, GameObject3D implementation  

### In Progress
+ 🔄 Improving GLRenderPipelineDemo rendering performance  
+ 🔄 Loading bones data from 3D model files using Assimp  

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

## Demos

### GLRenderPipelineDemo

#### Features
+ [x] Using RenderPipeline and RenderPass in rendering  
+ [x] Implement GeometryBuffer for deferred rendering  
+ [x] Implement CSM (Cascaded Shadow Maps) for shadow maps  
+ [x] Using possion disk in PCF shadow sample  
+ [x] Basic Blinn-Phong lighting model  
+ [x] Free perspective camera (moving mouse to rotate view, pressing wasd,shift to move position)  
+ [x] Configured assembly trimming and enabled AOT for publish  

#### Screenshots

<img width="1332" height="850" alt="img_1" src="https://github.com/user-attachments/assets/ae7bcaea-88c2-46d7-9f0c-886bf57c7e96" />
<img width="1332" height="850" alt="img_2" src="https://github.com/user-attachments/assets/c056b61d-71fc-427b-a3a2-d493dd59f1df" />

## Roadmap
- [ ] Complete GUI system
- [ ] Engine game object implementation
- [ ] BepuPhysics support
- [ ] D3D12 implementation
- [ ] Vulkan implementation

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
