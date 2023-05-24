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


#include "TextureWebGPUImpl.hpp"

#include "RenderDeviceWebGPUImpl.hpp"

namespace Diligent
{

WGPUTextureDescriptor TextureDescToWGPUTextureDescriptor(const TextureDesc& Desc, const RenderDeviceWebGPUImpl* pRenderDevice) noexcept
{

    WGPUTextureDescriptor TextureDesc{};
    if (Desc.Is1D())
        TextureDesc.dimension = WGPUTextureDimension_1D;
    else if (Desc.Is2D())
        TextureDesc.dimension = WGPUTextureDimension_2D;
    else if (Desc.Is3D())
        TextureDesc.dimension = WGPUTextureDimension_3D;
    else
        LOG_ERROR_AND_THROW("Unknown texture type");

    return TextureDesc;
}


TextureWebGPUImpl::TextureWebGPUImpl(IReferenceCounters*        pRefCounters,
                                     FixedBlockMemoryAllocator& TexViewObjAllocator,
                                     RenderDeviceWebGPUImpl*    pDevice,
                                     const TextureDesc&         Desc,
                                     const TextureData*         pInitData) :
    // clang-format off
    TTextureBase
    {
        pRefCounters,
        TexViewObjAllocator,
        pDevice,
        Desc
    }
// clang-format on
{

    if (m_Desc.Usage == USAGE_IMMUTABLE && (pInitData == nullptr || pInitData->pSubResources == nullptr))
        LOG_ERROR_AND_THROW("Immutable textures must be initialized with data at creation time: pInitData can't be null");

    if (m_Desc.Usage == USAGE_IMMUTABLE || m_Desc.Usage == USAGE_DEFAULT || m_Desc.Usage == USAGE_DYNAMIC)
    {
        
    }
    else if (m_Desc.Usage == USAGE_STAGING)
    {
        
    }
    else
    {
        UNSUPPORTED("Unsupported usage ", GetUsageString(m_Desc.Usage));
    }

    /*
    WGPUTextureDescriptor TextureDesc = TextureDescToWGPUTextureDescriptor(m_Desc, pDevice);
    m_wgpuTexture.Reset(wgpuDeviceCreateTexture(pDevice->GetWebGPUDevice(), &TextureDesc));

    WGPUQueue CmdQueue = wgpuDeviceGetQueue(pDevice->GetWebGPUDevice());

    WGPUBufferDescriptor BufferDesc{};
    WebGPUBufferWrapper StagingBuffer{wgpuDeviceCreateBuffer(pDevice->GetWebGPUDevice(), &BufferDesc)};
    wgpuBufferGetMappedRange(StagingBuffer.Get(), 0, 0);
    wgpuBufferUnmap(StagingBuffer.Get());

    WGPUCommandEncoderDescriptor EncoderDesc{};
    WGPUCommandEncoder CmdEncoder = wgpuDeviceCreateCommandEncoder(pDevice->GetWebGPUDevice(), &EncoderDesc);
    wgpuCommandEncoderCopyBufferToTexture(CmdEncoder, )


    WGPUCommandBufferDescriptor CmdBufferDesc{};
    WGPUCommandBuffer CmdBuffer = wgpuCommandEncoderFinish(CmdEncoder, &CmdBufferDesc);


    wgpuQueueSubmit(CmdQueue, 1, &CmdBuffer);
    */
}

} // namespace Diligent
