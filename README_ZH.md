# Vertix

[![C++ Standard](https://img.shields.io/badge/C++-20-blue?style=flat&logo=c%252B%252B&logoColor=white)](https://en.cppreference.com/w/cpp/20.html)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

一个使用 C++ 编写的轻量级 3D 游戏开发框架，面向 Windows 平台，基于 Direct3D 12 (D3D12)。

## 特性
- 基础的 D3D12 对象封装（交换链、命令列表、描述符堆、常量缓冲区、结构化缓冲区等）
- 基础的 3D 场景类型（透视相机、GameObject3D、SceneObject3D 等）
- 提供基础渲染资源的管理和异步加载功能（模型、纹理、材质等）
- 基础输入设备，基于 GameInput 实现（鼠标设备、键盘设备等）
- 示例/演示项目（Examples/D3D12GameDemo, Examples/D3D12ImGuiDemo, Examples/D3D12RenderPipelineDemo）

## 进行中的工作
- 使用计算着色器实现 Mipmap 生成器，在加载不包含 mipmap 的纹理格式时生成 mipmap。
- 改进 Mesh 和 Model 以支持骨骼动画。
- 为 GameObject3D 和 SceneObject3D 提供场景资源管理器，以便后续实现场景物体剔除等优化技术。
- 在 Vertix.Engine 中实现更多光源类型。
- 在 RenderPipelineDemo 中实现天空盒、多光源以及其它高级渲染效果。

## 前置要求
- Windows 10 / Windows 11
- Visual Studio 2022 或更高版本（包含“使用 C++ 的桌面开发”工作负载）
- Windows 10 SDK（或与 VS 版本匹配的 SDK）
- CMake（仓库的 CMakeLists 要求 `cmake_minimum_required(VERSION 4.0)` — 请确保您的 CMake 版本满足项目要求；如果环境中的 CMake 版本较旧，请升级到兼容版本）
- PowerShell（用于提供的获取脚本）
- 支持 Direct3D 12 的 GPU

## 依赖项
部分第三方头文件和库未提交到仓库，需要通过辅助脚本获取：
- Assimp（用于模型导入）
- d3dx12 / 辅助头文件 — 通过包含的脚本拉取
PowerShell 脚本 `fetch-includes.ps1` 和 `fetch-libraries.ps1` 将获取 `includes.json` 和 `libraries.json` 中列出的所需文件。

## 快速开始

1. 克隆仓库
   ```
   git clone https://github.com/natsurainko/Vertix.git
   cd Vertix
   ```

2. 获取第三方包含文件和库文件（使用 PowerShell）
   - 在仓库根目录打开 PowerShell 并运行：
     ``` bash
     .\fetch-includes.ps1
     .\fetch-libraries.ps1
     ```

3. 使用 CMake 配置和构建（以 Visual Studio 生成器为例）
   - 使用 Visual Studio：安装 C++ 开发工作负载，然后将项目文件夹作为 CMake 项目打开。
   - 使用 CLion：将项目文件夹作为 CMake 项目打开，并配置 CMake 以使用 Visual Studio 工具链。

4. 运行演示程序
   - 示例项目 `D3D12GameDemo` 位于 `Examples/D3D12GameDemo` 下。构建完成后，其可执行文件和相关资源将输出到构建目录的 bin 文件夹（CMake 会通过后期构建步骤复制资源文件夹）。

## 示例：启动应用程序
通过继承 `Vertix::GameWindow` 实现一个自定义窗口：

```cpp
#include "Windowing/GameWindow.h"

class DemoMainWindow : public Vertix::GameWindow {
public:
    // 您应保留此带有 `const Vertix::WindowOptions &options` 参数的构造函数，
    // 以便可以使用 `GameApplicationBuilder Vertix::GameApplicationBuilder::ConfigureWindow(const WindowOptions &options)` 方法。
    explicit DemoMainWindow(const Vertix::WindowOptions &options) : GameWindow(options) {}
};
```

然后在 `WinMain` 中使用 `Vertix::GameApplicationBuilder`：

```cpp
#include "DemoMainWindow.h"
#include "Hosting/GameApplicationBuilder.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, const int nShowCmd) {
    Vertix::WindowOptions windowOptions = Vertix::WindowOptions::GetDefaultWindowOptions();
    windowOptions.windowTitle = L"D3D12GameDemo.MainWindow";
    windowOptions.windowSize = { 1280, 720 };
    // 其他选项...

    return Vertix::GameApplicationBuilder(hInstance, lpCmdLine, nShowCmd)
        .ConfigureWindow<DemoMainWindow>(windowOptions)
        .Build()
        .Run();
}
```

## 演示程序

### D3D12GameDemo
- 路径：`Examples/D3D12GameDemo`
- 该演示展示了使用该框架进行基础渲染，并包含了示例资源。

#### 截图
<img width="1332" height="802" alt="D3D12GameDemo" src="https://github.com/user-attachments/assets/64dd3635-33ee-4b9b-abb5-4bcbe84db284" />

### D3D12ImGuiDemo
- 路径：`Examples/D3D12ImGuiDemo`
- 该演示展示了如何在框架内调用 ImGui。

#### 截图
<img width="1332" height="802" alt="D3D12ImGuiDemo" src="https://github.com/user-attachments/assets/d046f6c7-6e74-4641-b4ee-971ae4b6a86f" />

### D3D12RenderPipelineDemo
- 路径：`Examples/D3D12RenderPipelineDemo`
- 一个综合性的演示，展示了自定义 D3D12 渲染管线，并采用了多种高级渲染技术。
- 特性包括：
  - **延迟渲染**：多通道渲染，包含几何通道和光照通道，以高效处理多光源。
  - **纹理、材质、模型池**：使用库中的 ResourcePool 管理渲染资源。
  - **异步资源加载器**：使用 `TextureAsyncLoader` 和 `ModelAsyncLoader` 在后台线程上异步批量加载资源。
  - **无绑定纹理**：采用 GPU 驱动的渲染，通过无绑定纹理访问提升性能。
  - **级联阴影映射**：跨多个视锥体层级实现高效的阴影映射。
  - **PCSS 软阴影**：使用百分比渐近软阴影实现逼真的阴影半影区效果。
  - **基于 PBR 纹理的光照**：使用基于纹理的材质属性进行基于物理的渲染。
  - **HBAO (基于地平线的环境光遮蔽)**：使用 HBAO 提高阴影环境光的质量。

#### 截图
<img width="1332" height="850" alt="RenderPipelineDemo" src="https://github.com/user-attachments/assets/61273dad-4bba-4806-a3ee-bec4fbd0d3ef" />

#### 说明
- **Shader Model 6.6 要求**：GeometryPass 着色器需要 Shader Model 6.6 支持。
- **DXCompiler 依赖**：此演示需要 DXCompiler 库。其 DLL 文件位于依赖文件夹中，但需要手动复制到输出目录。请确保已正确安装 Windows SDK 以提供所需的 `.lib` 文件。

## 说明与提示
- 顶层的 `CMakeLists.txt` 将 `CMAKE_CXX_STANDARD` 设置为 20 — 项目使用了 C++20 的特性。
- 如果遇到缺少头文件或库的错误，请确保 `fetch-includes.ps1` 和 `fetch-libraries.ps1` 已成功运行，并且 `includes` / `libraries` 目录存在。
- 项目使用了系统库 `d3d12`、`dxgi`、`d3dcompiler`。请确保您的 Windows SDK 提供了这些库。

## 贡献
- 欢迎提交 Issue 和 PR。请清晰描述更改内容，并在适用时提供构建说明以供复现。
- 如果添加的功能依赖于额外的第三方库，请相应地更新 `includes.json` / `libraries.json` 以及获取脚本。

## 许可证
本项目采用 MIT 许可证 — 有关详细信息，请参阅 [LICENSE](LICENSE) 文件。
