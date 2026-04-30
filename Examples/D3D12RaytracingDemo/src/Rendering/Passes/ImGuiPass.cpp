//
// Created by Natsurainko on 2026/3/7.
//

#include "ImGuiPass.h"

#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/backends/imgui_impl_win32.h>

#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Graphics/SwapChain.h"
#include "Vertix/Windowing/GameWindow.h"

ImGuiPass::ImGuiPass(const Vertix::GameWindow *window) : window(window) {
    swapChain = window->GetSwapChain();
}

ImGuiPass::~ImGuiPass() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    delete imguiSrvDescriptorHeap;
}

void ImGuiPass::Initialize(
    Vertix::GraphicsDevice* device,
    RenderContext *context)
{
    RenderPass::Initialize(device, context);

    if (!imguiSrvDescriptorHeap) {
        imguiSrvDescriptorHeap = new Vertix::DescriptorHeap(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, true);
    }

    ImGui_ImplWin32_EnableDpiAwareness();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigDebugIsDebuggerPresent = true;

    ImGui::StyleColorsDark();

    const float dpiScale = ImGui_ImplWin32_GetDpiScaleForMonitor(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(dpiScale);
    style.FontScaleDpi = dpiScale;

    ImGui_ImplWin32_Init(window->GetWindowHandle());

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = graphicsDevice->GetD3D12Device().Get();
    init_info.CommandQueue = graphicsDevice->GetDefaultD3D12CommandQueue().Get();
    init_info.NumFramesInFlight = static_cast<int>(swapChain->GetFrameCount());
    init_info.RTVFormat = swapChain->GetRenderTargetFormat();
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    init_info.SrvDescriptorHeap = imguiSrvDescriptorHeap->GetDescriptorHeap().Get();
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return imguiSrvDescriptorHeap->AllocDescriptorHandle(*out_cpu_handle, *out_gpu_handle); };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE) { return imguiSrvDescriptorHeap->FreeDescriptorHandle(cpu_handle); };
    ImGui_ImplDX12_Init(&init_info);
}

void ImGuiPass::Execute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    {
        static bool _enableVSync = true;
        bool enableVSync = _enableVSync;

        ImGui::Begin("D3D12 Raytracing Demo");
        {
            ImGui::Text("Use WASD, Left Shift, and Space to move the camera.");
            ImGui::TextLinkOpenURL("GitHub Repository", "https://github.com/natsurainko/Vertix");

            ImGui::Checkbox("Enable VSync", &enableVSync);
            ImGui::SameLine();
            ImGui::Text("%.0f FPS (%.2f ms/frame)", io->Framerate, 1000.0f / io->Framerate);

            ImGui::SeparatorText("Camera");
            ImGui::InputFloat3("Position", const_cast<float*>(reinterpret_cast<const float*>(&renderContext->perspectiveCamera.GetPosition())), "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::InputFloat4("Orientation", const_cast<float*>(reinterpret_cast<const float*>(&renderContext->perspectiveCamera.GetOrientation())), "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::SeparatorText("Directional Light");
            ImGui::SliderFloat3("Direction", reinterpret_cast<float*>(&renderContext->LightConstants.LightDirection), 1.0f, -1.0f, "%.3f");
            ImGui::ColorEdit3("Light Color", reinterpret_cast<float*>(&renderContext->LightConstants.LightColor));
            ImGui::SliderFloat("Light Intensity", &renderContext->LightConstants.LightIntensity, 0.0f, 10.0f, "%.3f");
            ImGui::SliderFloat("Ambient Intensity", &renderContext->LightConstants.AmbientIntensity, 0.0f, 1.0f, "%.3f");
        }
        ImGui::End();

        if (enableVSync != _enableVSync) {
            swapChain->SetEnableVSync(enableVSync);
            _enableVSync = enableVSync;
        }
    }
    ImGui::Render();

    commandList->OMSetRenderTargets(1, &swapChain->GetCurrentFrameRenderTargetHandle(), FALSE, nullptr);
    commandList->SetDescriptorHeaps(1, imguiSrvDescriptorHeap->GetDescriptorHeap().GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
}
