//
// Created by Natsurainko on 2026/5/28.
//

#pragma once

#include <dxgi1_6.h>
#include <d3d12/d3d12.h>

#ifndef VERTIX_D3D12_DEVICE_VERSION
#define VERTIX_D3D12_DEVICE_VERSION 10
#endif

#ifndef VERTIX_D3D12_COMMAND_LIST_VERSION
#define VERTIX_D3D12_COMMAND_LIST_VERSION 5
#endif

#ifndef VERTIX_D3D12_COMMAND_QUEUE_VERSION
#define VERTIX_D3D12_COMMAND_QUEUE_VERSION 1
#endif

#ifndef VERTIX_DXGI_FACTORY_VERSION
#define VERTIX_DXGI_FACTORY_VERSION 7
#endif

#ifndef VERTIX_DXGI_ADAPTER_VERSION
#define VERTIX_DXGI_ADAPTER_VERSION 1
#endif

#ifndef VERTIX_DXGI_SWAP_CHAIN_VERSION
#define VERTIX_DXGI_SWAP_CHAIN_VERSION 4
#endif

#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
#include <dxgidebug.h>

#ifndef VERTIX_D3D12_DEBUG_VERSION
#define VERTIX_D3D12_DEBUG_VERSION 5
#endif

#ifndef VERTIX_DXGI_DEBUG_VERSION
#define VERTIX_DXGI_DEBUG_VERSION 1
#endif

#ifndef VERTIX_D3D12_DEBUG_DEVICE_VERSION
#define VERTIX_D3D12_DEBUG_DEVICE_VERSION 2
#endif

#endif

namespace Vertix::D3D12Interface {
    // ID3D12Device
#if VERTIX_D3D12_DEVICE_VERSION >= 14
    using Device = ID3D12Device14;
#elif VERTIX_D3D12_DEVICE_VERSION >= 13
    using Device = ID3D12Device13;
#elif VERTIX_D3D12_DEVICE_VERSION >= 12
    using Device = ID3D12Device12;
#elif VERTIX_D3D12_DEVICE_VERSION >= 11
    using Device = ID3D12Device11;
#elif VERTIX_D3D12_DEVICE_VERSION >= 10
    using Device = ID3D12Device10;
#elif VERTIX_D3D12_DEVICE_VERSION >= 9
    using Device = ID3D12Device9;
#elif VERTIX_D3D12_DEVICE_VERSION >= 8
    using Device = ID3D12Device8;
#elif VERTIX_D3D12_DEVICE_VERSION >= 7
    using Device = ID3D12Device7;
#elif VERTIX_D3D12_DEVICE_VERSION >= 6
    using Device = ID3D12Device6;
#elif VERTIX_D3D12_DEVICE_VERSION >= 5
    using Device = ID3D12Device5;
#elif VERTIX_D3D12_DEVICE_VERSION >= 4
    using Device = ID3D12Device4;
#elif VERTIX_D3D12_DEVICE_VERSION >= 3
    using Device = ID3D12Device3;
#elif VERTIX_D3D12_DEVICE_VERSION >= 2
    using Device = ID3D12Device2;
#elif VERTIX_D3D12_DEVICE_VERSION >= 1
    using Device = ID3D12Device1;
#else
    using Device = ID3D12Device;
#endif

    // ID3D12GraphicsCommandList
#if VERTIX_D3D12_COMMAND_LIST_VERSION >= 9
    using CommandList = ID3D12GraphicsCommandList9;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 8
    using CommandList = ID3D12GraphicsCommandList8;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 7
    using CommandList = ID3D12GraphicsCommandList7;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 6
    using CommandList = ID3D12GraphicsCommandList6;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 5
    using CommandList = ID3D12GraphicsCommandList5;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 4
    using CommandList = ID3D12GraphicsCommandList4;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 3
    using CommandList = ID3D12GraphicsCommandList3;
#elif VERTIX_D3D12_COMMAND_LIST_VERSION >= 2
    using CommandList = ID3D12GraphicsCommandList2;
#else
    using CommandList = ID3D12GraphicsCommandList1;
#endif

    // ID3D12CommandQueue
#if VERTIX_D3D12_COMMAND_QUEUE_VERSION >= 1
    using CommandQueue = ID3D12CommandQueue1;
#else
    using CommandQueue = ID3D12CommandQueue;
#endif

    using CommandAllocator = ID3D12CommandAllocator;

    // IDXGIFactory
#if VERTIX_DXGI_FACTORY_VERSION >= 7
    using DXGIFactory = IDXGIFactory7;
#elif VERTIX_DXGI_FACTORY_VERSION >= 6
    using DXGIFactory = IDXGIFactory6;
#elif VERTIX_DXGI_FACTORY_VERSION >= 5
    using DXGIFactory = IDXGIFactory5;
#elif VERTIX_DXGI_FACTORY_VERSION >= 4
    using DXGIFactory = IDXGIFactory4;
#elif VERTIX_DXGI_FACTORY_VERSION >= 3
    using DXGIFactory = IDXGIFactory3;
#else
    using DXGIFactory = IDXGIFactory2;
#endif

    // IDXGIAdapter
#if VERTIX_DXGI_ADAPTER_VERSION >= 4
    using DXGIAdapter = IDXGIAdapter4;
#elif VERTIX_DXGI_ADAPTER_VERSION >= 3
    using DXGIAdapter = IDXGIAdapter3;
#elif VERTIX_DXGI_ADAPTER_VERSION >= 2
    using DXGIAdapter = IDXGIAdapter2;
#elif VERTIX_DXGI_ADAPTER_VERSION >= 1
    using DXGIAdapter = IDXGIAdapter1;
#else
    using DXGIAdapter = IDXGIAdapter;
#endif

    // IDXGISwapChain
#if VERTIX_DXGI_SWAP_CHAIN_VERSION >= 4
    using DXGISwapChain = IDXGISwapChain4;
#else
    using DXGISwapChain = IDXGISwapChain3;
#endif

#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
    // ID3D12Debug
#if VERTIX_D3D12_DEBUG_VERSION >= 6
    using Debug = ID3D12Debug6;
#elif VERTIX_D3D12_DEBUG_VERSION >= 5
    using Debug = ID3D12Debug5;
#elif VERTIX_D3D12_DEBUG_VERSION >= 4
    using Debug = ID3D12Debug4;
#elif VERTIX_D3D12_DEBUG_VERSION >= 3
    using Debug = ID3D12Debug3;
#elif VERTIX_D3D12_DEBUG_VERSION >= 2
    using Debug = ID3D12Debug2;
#elif VERTIX_D3D12_DEBUG_VERSION >= 1
    using Debug = ID3D12Debug1;
#else
    using Debug = ID3D12Debug;
#endif

    // IDXGIDebug
#if VERTIX_DXGI_DEBUG_VERSION >= 1
    using DXGIDebug = IDXGIDebug1;
#else
    using DXGIDebug = IDXGIDebug;
#endif

    // ID3D12DebugDevice
#if VERTIX_D3D12_DEBUG_DEVICE_VERSION >= 2
    using DebugDevice = ID3D12DebugDevice2;
#elif VERTIX_D3D12_DEBUG_DEVICE_VERSION >= 1
    using DebugDevice = ID3D12DebugDevice1;
#else
    using DebugDevice = ID3D12DebugDevice;
#endif

#endif
}
