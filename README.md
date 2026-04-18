# Vertix

[![C++ Standard](https://img.shields.io/badge/C++-20-blue?style=flat&logo=c%252B%252B&logoColor=white)](https://en.cppreference.com/w/cpp/20.html)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A lightweight 3D game development framework written in C++, targeting Windows with Direct3D 12 (D3D12).

## Features
- Basic D3D12 object wrappers (SwapChain, CommandList, DescriptorHeap, ConstantBuffer, StructuredBuffer etc.)
- Basic 3D scene types (PerspectiveCamera, GameObject3D, SceneObject3D, etc.)
- Provides management and asynchronous loading of basic rendering resources. (Model, Texture, Material, etc.)
- Generate mipmaps for texture formats that do not have mipmaps using computational shaders.
- Basic input devices, implemented using GameInput (MouseDevice, KeyboardDevice, ...)
- Example/demo project (Examples/D3D12GameDemo, Examples/D3D12ImGuiDemo, Examples/D3D12RenderPipelineDemo)
- Provides convenient HLSL compilation and embedding for CMake (CMake function `add_hlsl_shaders`)

## In Progress
- Improve the Mesh and Model to enable skeletal animation
- Provide a scene resource manager for GameObject3D and SceneObject3D to facilitate subsequent optimization techniques such as scene object culling.
- Implementation of more light source types in Vertix.Engine
- Implementation of skybox, multi-point light sources, and other advanced rendering effects in RenderPipelineDemo

## Prerequisites
- Windows 10 / Windows 11
- Visual Studio 2022 or later (Desktop development with C++)
- Windows 10 SDK (or matching SDK for your VS)
- CMake (repository CMakeLists requests `cmake_minimum_required(VERSION 4.0)` — ensure your CMake version meets the project requirement; if your environment has an older CMake, upgrade to a compatible version)
- A Direct3D 12 capable GPU

## Dependencies
All third-party headers and libraries are located in the submodule at [Vertix.Dependencies](https://github.com/natsurainko/Vertix.Dependencies).  
**Please ensure that the submodule is cloned completely.**  
A list of references is provided within the submodule repository.

## Quick Start

1. Clone the repository
   ```
   git clone https://github.com/natsurainko/Vertix.git --recurse-submodules
   cd Vertix
   ```

2. Configure and build with CMake (Visual Studio generator example)
   - Using Visual Studio: Install the C++ development workload and open the project folder as a CMake project.
   - Using Clion: Open the project folder as a CMake project and configure CMake to use the Visual Studio toolchain

3. Run the demo
   - The example project `D3D12GameDemo` is added under `Examples/D3D12GameDemo`. After building, the executable and its assets will be output to the build bin folder (CMake will copy the assets folder as a post-build step).

## Example: launching an application
Implement a custom window by inheriting from `Vertix::GameWindow`:

```cpp
#include "Windowing/GameWindow.h"

class DemoMainWindow : public Vertix::GameWindow {
public:
    // You should retain this constructor with the `const Vertix::WindowOptions &options` parameter,
    // so that you can use `GameApplicationBuilder Vertix::GameApplicationBuilder::ConfigureWindow(const WindowOptions &options)` method.
    explicit DemoMainWindow(const Vertix::WindowOptions &options) : GameWindow(options) {}
};
```

Then use `Vertix::GameApplicationBuilder` in `WinMain`:

```cpp
#include "DemoMainWindow.h"
#include "Hosting/GameApplicationBuilder.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, const int nShowCmd) {
    Vertix::WindowOptions windowOptions = Vertix::WindowOptions::GetDefaultWindowOptions();
    windowOptions.windowTitle = L"D3D12GameDemo.MainWindow";
    windowOptions.windowSize = { 1280, 720 };
    // other options...

    return Vertix::GameApplicationBuilder(hInstance, lpCmdLine, nShowCmd)
        .ConfigureWindow<DemoMainWindow>(windowOptions)
        .Build()
        .Run();
}
```

## Demos

### D3D12GameDemo
- Path: `Examples/D3D12GameDemo`
- The demo shows basic rendering using the framework and includes sample assets.

#### Screenshots
<img width="1332" height="802" alt="D3D12GameDemo" src="https://github.com/user-attachments/assets/64dd3635-33ee-4b9b-abb5-4bcbe84db284" />

### D3D12ImGuiDemo
- Path: `Examples/D3D12ImGuiDemo`
- The demo demonstrates how to call ImGui within a framework.

#### Screenshots
<img width="1332" height="802" alt="D3D12ImGuiDemo" src="https://github.com/user-attachments/assets/d046f6c7-6e74-4641-b4ee-971ae4b6a86f" />

### D3D12RenderPipelineDemo
- Path: `Examples/D3D12RenderPipelineDemo`
- A comprehensive demonstration of a custom D3D12 rendering pipeline with advanced rendering techniques.
- Features include:
  - **Deferred Rendering**: Multi-pass rendering with geometry and lighting passes for efficient handling of multiple light sources.
  - **Texture, Material, Model Pools**: Use the library's ResourcePool to manage rendering resources.
  - **Asynchronous Resource Loader**: Use `TextureAsyncLoader` and `ModelAsyncLoader` to asynchronously batch load resources on a background thread..
  - **Bindless Texturing**: GPU-driven rendering with bindless texture access for improved performance.
  - **Cascaded Shadow Mapping**: Efficient shadow mapping across multiple view frustum levels.
  - **PCSS Soft Shadows**: Percentage-Closer Soft Shadows for realistic shadow penumbra.
  - **PBR Texture-Based Lighting**: Physically-Based Rendering using texture-based material properties.
  - **HBAO (Horizon-Based Ambient Occlusion)**: Use HBAO to improve the quality of shaded ambient light.

#### Screenshots
<img width="1332" height="850" alt="RenderPipelineDemo" src="https://github.com/user-attachments/assets/b15f48b2-8a57-46f2-bdae-4fa3daf35973" />

#### Notes
- **Shader Model 6.6 Requirement**: The GeometryPass shader requires Shader Model 6.6 support.
- **DXCompiler Dependency**: This demo requires the DXCompiler library. The DLL is provided in the dependencies folder but must be manually copied to the output directory. Ensure the Windows SDK is properly installed to provide the necessary `.lib` files.

## Notes and tips
- The top-level `CMakeLists.txt` sets `CMAKE_CXX_STANDARD` to 20 — the project uses C++20 features.
- The project uses system libraries `d3d12`, `dxgi`, `d3dcompiler`. Make sure your Windows SDK provides them.

## Contributing
- Issues and PRs are welcome. Please provide a clear description of changes and, when applicable, build instructions for reproducing.

## License
This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
