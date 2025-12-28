> [!CAUTION]
> I am migrating to C++ and providing a game framework that only targets the Direct3D 12 graphics API. After careful consideration, I have come to realize that abstracting multiple graphics APIs (OpenGL, D3D11, D3D12, Vulkan) into a unified interface is nearly impossible — if at all possible, it would inevitably sacrifice performance on certain platforms or force the abstraction to adopt the style of a specific platform's API (which I am unwilling to do). Therefore, I have decided to focus exclusively on D3D12 for the Windows platform. For both development convenience and performance reasons, I have chosen C++ over C#. I will soon complete the first D3D12 demo and then re‑upload the framework. Stay tuned!

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
+ [x] Implement CSM (Cascaded Shadow Maps) for directional shadow maps  
+ [x] Implement PCSS (Percentage-Closer Soft Shadow) for directional shadow, using possion disk in PCF shadow sample
+ [x] Implement SSAO (Screen Space Ambient Occulsion)  
+ [x] Basic Blinn-Phong lighting model  
+ [x] Free perspective camera (moving mouse to rotate view, pressing wasd,shift to move position)  
+ [x] Configured assembly trimming and enabled AOT for publish  

#### Screenshots

<img width="1332" height="850" alt="img_1" src="https://github.com/user-attachments/assets/e4fd2e88-bcc7-4c41-a3c0-a9a8a6ea6fc4" />
<img width="1332" height="850" alt="img_2" src="https://github.com/user-attachments/assets/282815f6-cef5-4d30-89fa-414dd8215a1f" />

## Roadmap
- [ ] Complete GUI system
- [ ] Engine game object implementation
- [ ] BepuPhysics support
- [ ] D3D12 implementation
- [ ] Vulkan implementation

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
