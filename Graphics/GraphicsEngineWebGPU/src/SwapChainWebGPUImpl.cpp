/*
 *  Copyright 2019-2023 Diligent Graphics LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "EngineWebGPUImplTraits.hpp"
#include "SwapChainWebGPUImpl.hpp"
#include "RenderDeviceWebGPUImpl.hpp"
#include "DeviceContextWebGPUImpl.hpp"
#include "RenderPassWebGPUImpl.hpp"
#include "FramebufferWebGPUImpl.hpp"
#include "TextureViewWebGPUImpl.hpp"
#include "PipelineStateWebGPUImpl.hpp"
#include "PipelineResourceAttribsWebGPU.hpp"
#include "ShaderResourceCacheWebGPU.hpp"
#include "BufferWebGPUImpl.hpp"
#include "BufferViewWebGPUImpl.hpp"

//TODO
#include <wrl/module.h>

namespace Diligent
{

SwapChainWebGPUImpl::SwapChainWebGPUImpl(IReferenceCounters*      pRefCounters,
                                         const SwapChainDesc&     SCDesc,
                                         RenderDeviceWebGPUImpl*  pRenderDevice,
                                         DeviceContextWebGPUImpl* pDeviceContext,
                                         const NativeWindow&      Window) :
    // clang-format off
    TSwapChainBase
    {
        pRefCounters,
        pRenderDevice,
        pDeviceContext,
        SCDesc
    }
// clang-format on
{

#if PLATFORM_WIN32
    WGPUSurfaceDescriptorFromWindowsHWND SurfaceNativeDesc{};
    SurfaceNativeDesc.chain = {nullptr, WGPUSType_SurfaceDescriptorFromWindowsHWND};
    SurfaceNativeDesc.hwnd  = Window.hWnd;
    SurfaceNativeDesc.hinstance = GetModuleHandle(nullptr);
#elif PLATFORM_LINUX
    WGPUSurfaceDescriptorFromXcbWindow SurfaceNativeDesc{};
    SurfaceNativeDesc.chain      = {nullptr, WGPUSType_SurfaceDescriptorFromXcbWindow};
    SurfaceNativeDesc.connection = Window.pXCBConnection;
    SurfaceNativeDesc.window     = Window.WindowId;
#elif PLATFROM_MACOS
    WGPUSurfaceDescriptorFromMetalLayer SurfaceNativeDesc{};
    SurfaceNativeDesc.chain      = {nullptr, WGPUSType_SurfaceDescriptorFromMetalLayer};
    SurfaceNativeDesc.window     = Window.MetalLayer;
#elif PLATFORM_EMSCRIPTEN
    WGPUSurfaceDescriptorFromCanvasHTMLSelector SurfaceNativeDesc{};
    SurfaceNativeDesc.chain    = {nullptr, WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector};
    SurfaceNativeDesc.selector = Window.Selector;
#endif

    WGPUSurfaceDescriptor SurfaceDesc{};
    SurfaceDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&SurfaceNativeDesc);

    m_wgpuSurface.Reset(wgpuInstanceCreateSurface(pRenderDevice->GetWebGPUInstance(), &SurfaceDesc));
    if (!m_wgpuSurface)
        LOG_ERROR_AND_THROW("Failed to create OS-specific surface");

    WGPUSurfaceCapabilities SurfaceCapabilities{};
    wgpuSurfaceGetCapabilities(m_wgpuSurface.Get(), pRenderDevice->GetWebGPUAdapter(), &SurfaceCapabilities);

    std::vector<WGPUTextureFormat> Formats(SurfaceCapabilities.formatCount);
    std::vector<WGPUPresentMode>   PresentModes(SurfaceCapabilities.presentModeCount);
    std::vector<WGPUCompositeAlphaMode> CompositeAlphaModes(SurfaceCapabilities.alphaModeCount);
    SurfaceCapabilities.formats = Formats.data();
    SurfaceCapabilities.presentModes = PresentModes.data();
    SurfaceCapabilities.alphaModes   = CompositeAlphaModes.data();
    wgpuSurfaceGetCapabilities(m_wgpuSurface.Get(), pRenderDevice->GetWebGPUAdapter(), &SurfaceCapabilities);

    WGPUSwapChainDescriptor SwapChainDesc{};
    SwapChainDesc.width  = SCDesc.Width;
    SwapChainDesc.height = SCDesc.Height;
    m_wgpuSwapChain.Reset(wgpuDeviceCreateSwapChain(pRenderDevice->GetWebGPUDevice(), m_wgpuSurface.Get(), &SwapChainDesc));
    if (!m_wgpuSwapChain)
        LOG_ERROR_AND_THROW("Failed to create WebGPU swapchain");

}

IMPLEMENT_QUERY_INTERFACE(SwapChainWebGPUImpl, IID_SwapChainWebGPU, TSwapChainBase)

void SwapChainWebGPUImpl::Present(Uint32 SyncInterval)
{
    wgpuSwapChainPresent(m_wgpuSwapChain.Get());
}

void SwapChainWebGPUImpl::Resize(Uint32            NewWidth,
                                 Uint32            NewHeight,
                                 SURFACE_TRANSFORM NewPreTransform)
{
    
}

void SwapChainWebGPUImpl::SetFullscreenMode(const DisplayModeAttribs& DisplayMode)
{
    UNSUPPORTED("WebGPU does not support switching to the fullscreen mode");
}

void SwapChainWebGPUImpl::SetWindowedMode()
{
    UNSUPPORTED("WebGPU does not support switching to the windowed mode");
}

ITextureView* SwapChainWebGPUImpl::GetCurrentBackBufferRTV()
{
    return nullptr;
}

ITextureView* SwapChainWebGPUImpl::GetDepthBufferDSV()
{
    return nullptr;
}


} // namespace Diligent
