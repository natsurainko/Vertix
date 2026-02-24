# Vertix

[![C++ Standard](https://img.shields.io/badge/C++-20-blue?style=flat&logo=c%252B%252B&logoColor=white)](https://en.cppreference.com/w/cpp/20.html)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A lightweight 3D game development framework written in C++, targeting Windows with Direct3D 12 (D3D12).

## Features
- Basic D3D12 object wrappers (SwapChain, CommandList, DescriptorHeap, HlslShader, etc.)
- 3D model importing (based on Assimp)
- Basic 3D scene types (PerspectiveCamera, GameObject3D, SceneObject3D, etc.)
- Basic rendering resource types (Model, Texture, Material, etc.)
- Basic input devices (MouseDevice, KeyboardDevice, ...)
- Example/demo project (Examples/D3D12GameDemo, Examples/D3D12RenderPipelineDemo)

## In Progress
- Encapsulation of rendering resource management classes for models, textures, materials, and other rendering assets
- Implementation of more light source types in Vertix.Engine
- Implementation of skybox, multi-point light sources, and other advanced rendering effects in RenderPipelineDemo

## Prerequisites
- Windows 10 / Windows 11
- Visual Studio 2022 or later (Desktop development with C++)
- Windows 10 SDK (or matching SDK for your VS)
- CMake (repository CMakeLists requests `cmake_minimum_required(VERSION 4.0)` — ensure your CMake version meets the project requirement; if your environment has an older CMake, upgrade to a compatible version)
- PowerShell (for the provided fetch scripts)
- A Direct3D 12 capable GPU

## Dependencies
Some third-party headers and libraries are not committed to the repository and are fetched by helper scripts:
- Assimp (for model importing)
- d3dx12 / helper headers — pulled by the included scripts
The PowerShell scripts `fetch-includes.ps1` and `fetch-libraries.ps1` will fetch the required files listed in `includes.json` and `libraries.json`.

## Quick Start

1. Clone the repository
   ```
   git clone https://github.com/natsurainko/Vertix.git
   cd Vertix
   ```

2. Fetch third-party includes and libraries (PowerShell)  
   - Open PowerShell in the repository root and run:  
     ``` bash
     .\fetch-includes.ps1
     .\fetch-libraries.ps1
     ```

3. Configure and build with CMake (Visual Studio generator example)
   - Using Visual Studio: Install the C++ development workload and open the project folder as a CMake project.
   - Using Clion: Open the project folder as a CMake project and configure CMake to use the Visual Studio toolchain

4. Run the demo
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
- CMakeLists for the demo includes a post-build step to copy `assets` to the output directory.

#### Screenshots
<img width="1332" height="802" alt="249711d8a25366c6d4e8578ca973a85f" src="https://github.com/user-attachments/assets/64dd3635-33ee-4b9b-abb5-4bcbe84db284" />

### D3D12RenderPipelineDemo
- Path: `Examples/D3D12RenderPipelineDemo`
- A comprehensive demonstration of a custom D3D12 rendering pipeline with advanced rendering techniques.
- Features include:
  - **Deferred Rendering**: Multi-pass rendering with geometry and lighting passes for efficient handling of multiple light sources.
  - **Texture and Material Management**: Complete a simple system for storing and loading texture and material resources.
  - **Bindless Texturing**: GPU-driven rendering with bindless texture access for improved performance.
  - **Cascaded Shadow Mapping**: Efficient shadow mapping across multiple view frustum levels.
  - **PCSS Soft Shadows**: Percentage-Closer Soft Shadows for realistic shadow penumbra.
  - **PBR Texture-Based Lighting**: Physically-Based Rendering using texture-based material properties.
- CMakeLists for the demo includes a post-build step to copy `assets` to the output directory.

#### Screenshots
<img width="1332" height="850" alt="RenderPipelineDemo" src="https://github.com/user-attachments/assets/bd528d97-2150-4e43-862f-138113845014" />

#### Notes
- **Shader Model 6.6 Requirement**: The GeometryPass shader requires Shader Model 6.6 support.
- **DXCompiler Dependency**: This demo requires the DXCompiler library. The DLL is provided in the dependencies folder but must be manually copied to the output directory. Ensure the Windows SDK is properly installed to provide the necessary `.lib` files.

## Notes and tips
- The top-level `CMakeLists.txt` sets `CMAKE_CXX_STANDARD` to 20 — the project uses C++20 features.
- If you hit missing header/library errors, ensure `fetch-includes.ps1` and `fetch-libraries.ps1` ran successfully and the `includes`/`libraries` directories exist.
- The project uses system libraries `d3d12`, `dxgi`, `d3dcompiler`. Make sure your Windows SDK provides them.

## Contributing
- Issues and PRs are welcome. Please provide a clear description of changes and, when applicable, build instructions for reproducing.
- If adding features that depend on additional third-party libraries, update `includes.json` / `libraries.json` and the fetch scripts accordingly.

## License
This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
