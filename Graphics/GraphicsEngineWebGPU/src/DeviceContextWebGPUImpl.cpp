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

#include "DeviceContextWebGPUImpl.hpp"
#include "RenderDeviceWebGPUImpl.hpp"
#include "TextureWebGPUImpl.hpp"
#include "TextureViewWebGPUImpl.hpp"
#include "BufferWebGPUImpl.hpp"
#include "PipelineStateWebGPUImpl.hpp"
#include "PipelineResourceSignatureWebGPUImpl.hpp"
#include "PipelineResourceAttribsWebGPU.hpp"
#include "ShaderResourceCacheWebGPU.hpp"
#include "RenderPassWebGPUImpl.hpp"
#include "FramebufferWebGPUImpl.hpp"


namespace Diligent
{

DeviceContextWebGPUImpl::DeviceContextWebGPUImpl(IReferenceCounters*           pRefCounters,
                                                 RenderDeviceWebGPUImpl*       pDevice,
                                                 const EngineWebGPUCreateInfo& EngineCI,
                                                 const DeviceContextDesc&      Desc) :
    // clang-format off
    TDeviceContextBase
    {
        pRefCounters,
        pDevice,
        Desc
    }
// clang-format on
{
}

IMPLEMENT_QUERY_INTERFACE(DeviceContextWebGPUImpl, IID_DeviceContextWebGPU, TDeviceContextBase)

void DeviceContextWebGPUImpl::Begin(Uint32 ImmediateContextId) {}

void DeviceContextWebGPUImpl::SetPipelineState(IPipelineState* pPipelineState) {}

void DeviceContextWebGPUImpl::TransitionShaderResources(IPipelineState*         pPipelineState,
                                                        IShaderResourceBinding* pShaderResourceBinding) {}

void DeviceContextWebGPUImpl::CommitShaderResources(IShaderResourceBinding*        pShaderResourceBinding,
                                                    RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) {}

void DeviceContextWebGPUImpl::SetStencilRef(Uint32 StencilRef) {}

void DeviceContextWebGPUImpl::SetBlendFactors(const float* pBlendFactors) {}

void DeviceContextWebGPUImpl::SetVertexBuffers(Uint32                         StartSlot,
                                               Uint32                         NumBuffersSet,
                                               IBuffer**                      ppBuffers,
                                               const Uint64*                  pOffsets,
                                               RESOURCE_STATE_TRANSITION_MODE StateTransitionMode,
                                               SET_VERTEX_BUFFERS_FLAGS       Flags) {}
void DeviceContextWebGPUImpl::InvalidateState() {}

void DeviceContextWebGPUImpl::SetIndexBuffer(IBuffer*                       pIndexBuffer,
                                             Uint64                         ByteOffset,
                                             RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) {}

void DeviceContextWebGPUImpl::SetViewports(Uint32          NumViewports,
                                           const Viewport* pViewports,
                                           Uint32          RTWidth,
                                           Uint32          RTHeight) {}

void DeviceContextWebGPUImpl::SetScissorRects(Uint32 NumRects, const Rect* pRects, Uint32 RTWidth, Uint32 RTHeight) {}

void DeviceContextWebGPUImpl::SetRenderTargetsExt(const SetRenderTargetsAttribs& Attribs) {}

void DeviceContextWebGPUImpl::BeginRenderPass(const BeginRenderPassAttribs& Attribs) {}

void DeviceContextWebGPUImpl::NextSubpass() {}

void DeviceContextWebGPUImpl::EndRenderPass() {}

void DeviceContextWebGPUImpl::Draw(const DrawAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DrawIndexed(const DrawIndexedAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DrawIndirect(const DrawIndirectAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DrawIndexedIndirect(const DrawIndexedIndirectAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DrawMesh(const DrawMeshAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DrawMeshIndirect(const DrawMeshIndirectAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DispatchCompute(const DispatchComputeAttribs& Attribs) {}

void DeviceContextWebGPUImpl::DispatchComputeIndirect(const DispatchComputeIndirectAttribs& Attribs) {}

void DeviceContextWebGPUImpl::GetTileSize(Uint32& TileSizeX, Uint32& TileSizeY)
{
    DeviceContextBase<EngineWebGPUImplTraits>::GetTileSize(TileSizeX, TileSizeY);
}

void DeviceContextWebGPUImpl::ClearDepthStencil(ITextureView*                  pView,
                                                CLEAR_DEPTH_STENCIL_FLAGS      ClearFlags,
                                                float                          fDepth,
                                                Uint8                          Stencil,
                                                RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) {}

void DeviceContextWebGPUImpl::ClearRenderTarget(ITextureView*                  pView,
                                                const float*                   RGBA,
                                                RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) {}

void DeviceContextWebGPUImpl::UpdateBuffer(IBuffer*                       pBuffer,
                                           Uint64                         Offset,
                                           Uint64                         Size,
                                           const void*                    pData,
                                           RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) {}

void DeviceContextWebGPUImpl::CopyBuffer(IBuffer*                       pSrcBuffer,
                                         Uint64                         SrcOffset,
                                         RESOURCE_STATE_TRANSITION_MODE SrcBufferTransitionMode,
                                         IBuffer*                       pDstBuffer,
                                         Uint64                         DstOffset,
                                         Uint64                         Size,
                                         RESOURCE_STATE_TRANSITION_MODE DstBufferTransitionMode) {}

void DeviceContextWebGPUImpl::MapBuffer(IBuffer*  pBuffer,
                                        MAP_TYPE  MapType,
                                        MAP_FLAGS MapFlags,
                                        PVoid&    pMappedData) {}

void DeviceContextWebGPUImpl::UnmapBuffer(IBuffer* pBuffer, MAP_TYPE MapType) {}

void DeviceContextWebGPUImpl::UpdateTexture(ITexture*                      pTexture,
                                            Uint32                         MipLevel,
                                            Uint32                         Slice,
                                            const Box&                     DstBox,
                                            const TextureSubResData&       SubresData,
                                            RESOURCE_STATE_TRANSITION_MODE SrcBufferStateTransitionMode,
                                            RESOURCE_STATE_TRANSITION_MODE TextureStateTransitionMode) {}

void DeviceContextWebGPUImpl::CopyTexture(const CopyTextureAttribs& CopyAttribs) {}

void DeviceContextWebGPUImpl::MapTextureSubresource(ITexture*                 pTexture,
                                                    Uint32                    MipLevel,
                                                    Uint32                    ArraySlice,
                                                    MAP_TYPE                  MapType,
                                                    MAP_FLAGS                 MapFlags,
                                                    const Box*                pMapRegion,
                                                    MappedTextureSubresource& MappedData) {}

void DeviceContextWebGPUImpl::UnmapTextureSubresource(ITexture* pTexture, Uint32 MipLevel, Uint32 ArraySlice) {}

void DeviceContextWebGPUImpl::FinishCommandList(ICommandList** ppCommandList) {}

void DeviceContextWebGPUImpl::ExecuteCommandLists(Uint32 NumCommandLists, ICommandList* const* ppCommandLists) {}

void DeviceContextWebGPUImpl::EnqueueSignal(IFence* pFence, Uint64 Value) {}

void DeviceContextWebGPUImpl::DeviceWaitForFence(IFence* pFence, Uint64 Value) {}

void DeviceContextWebGPUImpl::WaitForIdle() {}

void DeviceContextWebGPUImpl::BeginQuery(IQuery* pQuery) {}

void DeviceContextWebGPUImpl::EndQuery(IQuery* pQuery) {}

void DeviceContextWebGPUImpl::Flush() {}

void DeviceContextWebGPUImpl::BuildBLAS(const BuildBLASAttribs& Attribs) {}

void DeviceContextWebGPUImpl::BuildTLAS(const BuildTLASAttribs& Attribs) {}

void DeviceContextWebGPUImpl::CopyBLAS(const CopyBLASAttribs& Attribs) {}

void DeviceContextWebGPUImpl::CopyTLAS(const CopyTLASAttribs& Attribs) {}

void DeviceContextWebGPUImpl::WriteBLASCompactedSize(const WriteBLASCompactedSizeAttribs& Attribs) {}

void DeviceContextWebGPUImpl::WriteTLASCompactedSize(const WriteTLASCompactedSizeAttribs& Attribs) {}

void DeviceContextWebGPUImpl::TraceRays(const TraceRaysAttribs& Attribs) {}

void DeviceContextWebGPUImpl::TraceRaysIndirect(const TraceRaysIndirectAttribs& Attribs) {}

void DeviceContextWebGPUImpl::UpdateSBT(IShaderBindingTable* pSBT, const UpdateIndirectRTBufferAttribs* pUpdateIndirectBufferAttribs) {}

void DeviceContextWebGPUImpl::BeginDebugGroup(const Char* Name, const float* pColor) {}

void DeviceContextWebGPUImpl::EndDebugGroup() {}

void DeviceContextWebGPUImpl::InsertDebugLabel(const Char* Label, const float* pColor) {}

void DeviceContextWebGPUImpl::SetShadingRate(SHADING_RATE          BaseRate,
                                             SHADING_RATE_COMBINER PrimitiveCombiner,
                                             SHADING_RATE_COMBINER TextureCombiner) {}

void DeviceContextWebGPUImpl::BindSparseResourceMemory(const BindSparseResourceMemoryAttribs& Attribs) {}

void DeviceContextWebGPUImpl::GenerateMips(ITextureView* pTexView) {}

void DeviceContextWebGPUImpl::FinishFrame() {}

void DeviceContextWebGPUImpl::TransitionResourceStates(Uint32 BarrierCount, const StateTransitionDesc* pResourceBarriers) {}

ICommandQueue* DeviceContextWebGPUImpl::LockCommandQueue() { return nullptr; }

void DeviceContextWebGPUImpl::UnlockCommandQueue() {}

void DeviceContextWebGPUImpl::ResolveTextureSubresource(ITexture*                               pSrcTexture,
                                                        ITexture*                               pDstTexture,
                                                        const ResolveTextureSubresourceAttribs& ResolveAttribs)
{
}

WGPUQueue DeviceContextWebGPUImpl::GetWebGPUQueue()
{
    return {};
}

} // namespace Diligent
