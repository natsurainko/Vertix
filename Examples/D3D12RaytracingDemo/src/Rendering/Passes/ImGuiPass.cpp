//
// Created by Natsurainko on 2026/3/7.
//

#include "ImGuiPass.h"

#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <../../../../../Vertix/include/Vertix/Windowing/SwapChain.h>

ImGuiPass::~ImGuiPass() {
    if (ImGui::GetCurrentContext() == nullptr) return;

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiPass::Initialize(ID3D12Device10 *device) {
    static Vertix::DescriptorHeap* sharedDescriptorHeap = renderContext->sharedDescriptorHeap;

    if (ImGui::GetCurrentContext() != nullptr) {
        io = &ImGui::GetIO();
        return;
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
    init_info.Device = device;
    init_info.CommandQueue = window->GetFrameCommandList()->GetD3D12CommandQueue().Get();
    init_info.NumFramesInFlight = 2;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    init_info.SrvDescriptorHeap = sharedDescriptorHeap->GetDescriptorHeap();
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
        const auto handle = sharedDescriptorHeap->AllocDescriptorHandle();
        *out_cpu_handle = handle.cpuHandle;
        *out_gpu_handle = handle.gpuHandle;
    };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE) {
        sharedDescriptorHeap->FreeDescriptorHandle(cpu_handle);
    };
    ImGui_ImplDX12_Init(&init_info);
}

void ImGuiPass::Execute(ID3D12GraphicsCommandList5* commandList) {
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
            ImGui::SliderFloat3("Direction", reinterpret_cast<float*>(&renderContext->lightConstants.LightDirection), 1.0f, -1.0f, "%.3f");
            ImGui::ColorEdit3("Light Color", reinterpret_cast<float*>(&renderContext->lightConstants.LightColor));
            ImGui::SliderFloat("Light Intensity", &renderContext->lightConstants.LightIntensity, 0.0f, 10.0f, "%.3f");
            ImGui::SliderFloat("Ambient Intensity", &renderContext->lightConstants.AmbientIntensity, 0.0f, 1.0f, "%.3f");
        }
        ImGui::End();

        if (enableVSync != _enableVSync) {
            swapChain->SetEnableVSync(enableVSync);
            _enableVSync = enableVSync;
        }
    }
    ImGui::Render();

    currentFrameRTV.SetRenderTarget(commandList);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}
