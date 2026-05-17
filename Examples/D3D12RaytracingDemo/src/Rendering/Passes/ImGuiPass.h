//
// Created by Natsurainko on 2026/3/7.
//

#ifndef VERTIX_IMGUIPASS_H
#define VERTIX_IMGUIPASS_H

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx12.h>
#include <imgui/backends/imgui_impl_win32.h>

#include "../RenderContext.h"
#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Rendering/Pipeline/RenderPass.h"
#include "Vertix/Windowing/GameWindow.h"

extern Vertix::DescriptorHeap* imguiSrvDescriptorHeap;

class ImGuiPass : public Vertix::RenderPass {
public:
    explicit ImGuiPass(
        const Vertix::GameWindow* window,
        RenderContext* renderContext)
    : swapChain(window->GetSwapChain()), renderContext(renderContext)
    {
        if (ImGui::GetCurrentContext() != nullptr) {
            io = &ImGui::GetIO();
            return;
        }

        const auto device = window->GetGraphicsDevice()->GetD3D12Device().Get();
        const auto commandQueue = window->GetFrameCommandList()->GetD3D12CommandQueue().Get();

        imguiSrvDescriptorHeap = new Vertix::DescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, true);

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
        init_info.CommandQueue = commandQueue;
        init_info.NumFramesInFlight = 2;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

        init_info.SrvDescriptorHeap = imguiSrvDescriptorHeap->GetDescriptorHeap();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
            const auto handle = imguiSrvDescriptorHeap->AllocDescriptorHandle();
            *out_cpu_handle = handle.cpuHandle;
            *out_gpu_handle = handle.gpuHandle;
        };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE) {
            return imguiSrvDescriptorHeap->FreeDescriptorHandle(cpu_handle);
        };
        ImGui_ImplDX12_Init(&init_info);
    }

    ~ImGuiPass() override {
        if (ImGui::GetCurrentContext() == nullptr) return;

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        delete imguiSrvDescriptorHeap;
    }

    void Initialize(ID3D12Device10* device) override {}
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    const Vertix::RenderResourceView<Vertix::RenderResourceViewType::RenderTarget>* currentFrameRTV = nullptr;

private:
    Vertix::SwapChain* swapChain;
    RenderContext* renderContext;
    ImGuiIO* io;
};

#endif //VERTIX_IMGUIPASS_H
