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


#include "RenderDeviceWebGPUImpl.hpp"
#include "DeviceContextWebGPUImpl.hpp"
#include "RenderPassWebGPUImpl.hpp"
#include "TextureWebGPUImpl.hpp"
#include "BufferWebGPUImpl.hpp"
#include "PipelineStateWebGPUImpl.hpp"
#include "PipelineResourceSignatureWebGPUImpl.hpp"
#include "PipelineResourceAttribsWebGPU.hpp"
#include "ShaderResourceCacheWebGPU.hpp"
#include "RenderPassWebGPUImpl.hpp"
#include "FramebufferWebGPUImpl.hpp"
#include "SamplerWebGPUImpl.hpp"
#include "FenceWebGPUImpl.hpp"
#include "QueryWebGPUImpl.hpp"

namespace Diligent
{

class BottomLevelASWebGPUImpl
{};

class TopLevelASWebGPUImpl
{};

class ShaderBindingTableWebGPUImpl
{};

class DeviceMemoryWebGPUImpl
{};

RenderDeviceWebGPUImpl::RenderDeviceWebGPUImpl(IReferenceCounters*           pRefCounters,
                                               IMemoryAllocator&             RawMemAllocator,
                                               IEngineFactory*               pEngineFactory,
                                               const EngineWebGPUCreateInfo& EngineCI,
                                               const GraphicsAdapterInfo&    AdapterInfo,
                                               WGPUInstance                  wgpuInstance,
                                               WGPUAdapter                   wgpuAdapter,
                                               WGPUDevice                    wgpuDevice) :

    // clang-format off
    TRenderDeviceBase
    {
        pRefCounters,
        RawMemAllocator,
        pEngineFactory,
        EngineCI,
        AdapterInfo
    },
    m_wgpuInstance(wgpuInstance),
    m_wgpuAdapter{wgpuAdapter},
    m_wgpuDevice{wgpuDevice}
// clang-format on
{
    m_DeviceInfo.Type = RENDER_DEVICE_TYPE_WEBGPU;
    m_DeviceInfo.Features = EnableDeviceFeatures(m_AdapterInfo.Features, EngineCI.Features);

}

IMPLEMENT_QUERY_INTERFACE(RenderDeviceWebGPUImpl, IID_RenderDeviceWebGPU, TRenderDeviceBase)

void RenderDeviceWebGPUImpl::CreateBuffer(const BufferDesc& BuffDesc,
                                          const BufferData* pBuffData,
                                          IBuffer**         ppBuffer)
{
}

void RenderDeviceWebGPUImpl::CreateShader(const ShaderCreateInfo& ShaderCI,
                                          IShader**               ppShader)
{
}

void RenderDeviceWebGPUImpl::CreateTexture(const TextureDesc& TexDesc,
                                           const TextureData* pData,
                                           ITexture**         ppTexture)
{
}

void RenderDeviceWebGPUImpl::CreateSampler(const SamplerDesc& SamplerDesc,
                                           ISampler**         ppSampler)
{
}

void RenderDeviceWebGPUImpl::CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& PSOCreateInfo,
                                                         IPipelineState**                       ppPipelineState)
{
}

void RenderDeviceWebGPUImpl::CreateComputePipelineState(const ComputePipelineStateCreateInfo& PSOCreateInfo,
                                                        IPipelineState**                      ppPipelineState)
{
}

void RenderDeviceWebGPUImpl::CreateRayTracingPipelineState(const RayTracingPipelineStateCreateInfo& PSOCreateInfo,
                                                           IPipelineState**                         ppPipelineState)
{
}

void RenderDeviceWebGPUImpl::CreateFence(const FenceDesc& Desc,
                                         IFence**         ppFence)
{
}

void RenderDeviceWebGPUImpl::CreateQuery(const QueryDesc& Desc,
                                         IQuery**         ppQuery)
{
}

void RenderDeviceWebGPUImpl::CreateRenderPass(const RenderPassDesc& Desc,
                                              IRenderPass**         ppRenderPass)
{
}

void RenderDeviceWebGPUImpl::CreateFramebuffer(const FramebufferDesc& Desc,
                                               IFramebuffer**         ppFramebuffer)
{
}

void RenderDeviceWebGPUImpl::CreateBLAS(const BottomLevelASDesc& Desc,
                                        IBottomLevelAS**         ppBLAS)
{
}

void RenderDeviceWebGPUImpl::CreateTLAS(const TopLevelASDesc& Desc,
                                        ITopLevelAS**         ppTLAS)
{
}

void RenderDeviceWebGPUImpl::CreateSBT(const ShaderBindingTableDesc& Desc,
                                       IShaderBindingTable**         ppSBT)
{
}

void RenderDeviceWebGPUImpl::CreatePipelineResourceSignature(const PipelineResourceSignatureDesc& Desc,
                                                             IPipelineResourceSignature**         ppSignature)
{
}

void RenderDeviceWebGPUImpl::CreateDeviceMemory(const DeviceMemoryCreateInfo& CreateInfo,
                                                IDeviceMemory**               ppMemory)
{
}

void RenderDeviceWebGPUImpl::CreatePipelineStateCache(const PipelineStateCacheCreateInfo& CreateInfo,
                                                      IPipelineStateCache**               ppPSOCache)
{
}

void RenderDeviceWebGPUImpl::IdleGPU()
{
}

SparseTextureFormatInfo RenderDeviceWebGPUImpl::GetSparseTextureFormatInfo(TEXTURE_FORMAT     TexFormat,
                                                                           RESOURCE_DIMENSION Dimension,
                                                                           Uint32             SampleCount) const
{
    UNSUPPORTED("GetSparseTextureFormatInfo is not supported in WebGPU");
    return {};
}

void RenderDeviceWebGPUImpl::TestTextureFormat(TEXTURE_FORMAT TexFormat)
{
    UNSUPPORTED("TestTextureFormat is not supported in WebGPU");
}

} // namespace Diligent
