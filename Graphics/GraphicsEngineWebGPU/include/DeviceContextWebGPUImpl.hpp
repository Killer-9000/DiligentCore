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

#pragma once

/// \file
/// Declaration of Diligent::DeviceContextWebGPUImpl class

#include "EngineWebGPUImplTraits.hpp"
#include "DeviceContextBase.hpp"
#include "TextureWebGPUImpl.hpp"
#include "BufferWebGPUImpl.hpp"
#include "PipelineStateWebGPUImpl.hpp"
#include "ShaderWebGPUImpl.hpp"
#include "FramebufferWebGPUImpl.hpp"

namespace Diligent
{

/// Device context implementation in WebGPU backend.
class DeviceContextWebGPUImpl final : public DeviceContextBase<EngineWebGPUImplTraits>
{
public:
    using TDeviceContextBase = DeviceContextBase;

    DeviceContextWebGPUImpl(IReferenceCounters*           pRefCounters,
                            RenderDeviceWebGPUImpl*       pDevice,
                            const EngineWebGPUCreateInfo& EngineCI,
                            const DeviceContextDesc&      Desc);

    void DILIGENT_CALL_TYPE QueryInterface(const INTERFACE_ID& IID, IObject** ppInterface) override;

    /// Implementation of IDeviceContext::Begin() in WebGPU backend.
    void DILIGENT_CALL_TYPE Begin(Uint32 ImmediateContextId) override;

    /// Implementation of IDeviceContext::SetPipelineState() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetPipelineState(IPipelineState* pPipelineState) override;

    /// Implementation of IDeviceContext::TransitionShaderResources() in WebGPU backend.
    void DILIGENT_CALL_TYPE TransitionShaderResources(IPipelineState*         pPipelineState,
                                                      IShaderResourceBinding* pShaderResourceBinding) override;

    /// Implementation of IDeviceContext::CommitShaderResources() in WebGPU backend.
    void DILIGENT_CALL_TYPE CommitShaderResources(IShaderResourceBinding*        pShaderResourceBinding,
                                                  RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) override;

    /// Implementation of IDeviceContext::SetStencilRef() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetStencilRef(Uint32 StencilRef) override;

    /// Implementation of IDeviceContext::SetBlendFactors() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetBlendFactors(const float* pBlendFactors = nullptr) override;

    /// Implementation of IDeviceContext::SetVertexBuffers() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetVertexBuffers(Uint32                         StartSlot,
                                             Uint32                         NumBuffersSet,
                                             IBuffer**                      ppBuffers,
                                             const Uint64*                  pOffsets,
                                             RESOURCE_STATE_TRANSITION_MODE StateTransitionMode,
                                             SET_VERTEX_BUFFERS_FLAGS       Flags) override;

    /// Implementation of IDeviceContext::InvalidateState() in WebGPU backend.
    void DILIGENT_CALL_TYPE InvalidateState() override;

    /// Implementation of IDeviceContext::SetIndexBuffer() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetIndexBuffer(IBuffer*                       pIndexBuffer,
                                           Uint64                         ByteOffset,
                                           RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) override;

    /// Implementation of IDeviceContext::SetViewports() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetViewports(Uint32          NumViewports,
                                         const Viewport* pViewports,
                                         Uint32          RTWidth,
                                         Uint32          RTHeight) override;

    /// Implementation of IDeviceContext::SetScissorRects() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetScissorRects(Uint32      NumRects,
                                            const Rect* pRects,
                                            Uint32      RTWidth,
                                            Uint32      RTHeight) override;

    /// Implementation of IDeviceContext::SetRenderTargetsExt() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetRenderTargetsExt(const SetRenderTargetsAttribs& Attribs) override;

    /// Implementation of IDeviceContext::BeginRenderPass() in WebGPU backend.
    void DILIGENT_CALL_TYPE BeginRenderPass(const BeginRenderPassAttribs& Attribs) override;

    /// Implementation of IDeviceContext::NextSubpass() in WebGPU backend.
    void DILIGENT_CALL_TYPE NextSubpass() override;

    /// Implementation of IDeviceContext::EndRenderPass() in WebGPU backend.
    void DILIGENT_CALL_TYPE EndRenderPass() override;

    /// Implementation of IDeviceContext::Draw() in WebGPU backend.
    void DILIGENT_CALL_TYPE Draw(const DrawAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DrawIndexed() in WebGPU backend.
    void DILIGENT_CALL_TYPE DrawIndexed(const DrawIndexedAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DrawIndirect() in WebGPU backend.
    void DILIGENT_CALL_TYPE DrawIndirect(const DrawIndirectAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DrawIndexedIndirect() in WebGPU backend.
    void DILIGENT_CALL_TYPE DrawIndexedIndirect(const DrawIndexedIndirectAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DrawMesh() in WebGPU backend.
    void DILIGENT_CALL_TYPE DrawMesh(const DrawMeshAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DrawMeshIndirect() in WebGPU backend.
    void DILIGENT_CALL_TYPE DrawMeshIndirect(const DrawMeshIndirectAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DispatchCompute() in WebGPU backend.
    void DILIGENT_CALL_TYPE DispatchCompute(const DispatchComputeAttribs& Attribs) override;

    /// Implementation of IDeviceContext::DispatchComputeIndirect() in WebGPU backend.
    void DILIGENT_CALL_TYPE DispatchComputeIndirect(const DispatchComputeIndirectAttribs& Attribs) override;

    /// Implementation of IDeviceContext::GetTileSize() in WebGPU backend.
    void DILIGENT_CALL_TYPE GetTileSize(Uint32& TileSizeX, Uint32& TileSizeY) override;

    /// Implementation of IDeviceContext::ClearDepthStencil() in WebGPU backend.
    void DILIGENT_CALL_TYPE ClearDepthStencil(ITextureView*                  pView,
                                              CLEAR_DEPTH_STENCIL_FLAGS      ClearFlags,
                                              float                          fDepth,
                                              Uint8                          Stencil,
                                              RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) override;

    /// Implementation of IDeviceContext::ClearRenderTarget() in WebGPU backend.
    void DILIGENT_CALL_TYPE ClearRenderTarget(ITextureView*                  pView,
                                              const float*                   RGBA,
                                              RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) override;

    /// Implementation of IDeviceContext::UpdateBuffer() in WebGPU backend.
    void DILIGENT_CALL_TYPE UpdateBuffer(IBuffer*                       pBuffer,
                                         Uint64                         Offset,
                                         Uint64                         Size,
                                         const void*                    pData,
                                         RESOURCE_STATE_TRANSITION_MODE StateTransitionMode) override;

    /// Implementation of IDeviceContext::CopyBuffer() in WebGPU backend.
    void DILIGENT_CALL_TYPE CopyBuffer(IBuffer*                       pSrcBuffer,
                                       Uint64                         SrcOffset,
                                       RESOURCE_STATE_TRANSITION_MODE SrcBufferTransitionMode,
                                       IBuffer*                       pDstBuffer,
                                       Uint64                         DstOffset,
                                       Uint64                         Size,
                                       RESOURCE_STATE_TRANSITION_MODE DstBufferTransitionMode) override;

    /// Implementation of IDeviceContext::MapBuffer() in WebGPU backend.
    void DILIGENT_CALL_TYPE MapBuffer(IBuffer*  pBuffer,
                                      MAP_TYPE  MapType,
                                      MAP_FLAGS MapFlags,
                                      PVoid&    pMappedData) override;

    /// Implementation of IDeviceContext::UnmapBuffer() in WebGPU backend.
    void DILIGENT_CALL_TYPE UnmapBuffer(IBuffer* pBuffer, MAP_TYPE MapType) override;

    /// Implementation of IDeviceContext::UpdateTexture() in WebGPU backend.
    void DILIGENT_CALL_TYPE UpdateTexture(ITexture*                      pTexture,
                                          Uint32                         MipLevel,
                                          Uint32                         Slice,
                                          const Box&                     DstBox,
                                          const TextureSubResData&       SubresData,
                                          RESOURCE_STATE_TRANSITION_MODE SrcBufferStateTransitionMode,
                                          RESOURCE_STATE_TRANSITION_MODE TextureStateTransitionMode) override;

    /// Implementation of IDeviceContext::CopyTexture() in WebGPU backend.
    void DILIGENT_CALL_TYPE CopyTexture(const CopyTextureAttribs& CopyAttribs) override;

    /// Implementation of IDeviceContext::MapTextureSubresource() in WebGPU backend.
    void DILIGENT_CALL_TYPE MapTextureSubresource(ITexture*                 pTexture,
                                                  Uint32                    MipLevel,
                                                  Uint32                    ArraySlice,
                                                  MAP_TYPE                  MapType,
                                                  MAP_FLAGS                 MapFlags,
                                                  const Box*                pMapRegion,
                                                  MappedTextureSubresource& MappedData) override;

    /// Implementation of IDeviceContext::UnmapTextureSubresource() in WebGPU backend.
    void DILIGENT_CALL_TYPE UnmapTextureSubresource(ITexture* pTexture, Uint32 MipLevel, Uint32 ArraySlice) override;

    /// Implementation of IDeviceContext::FinishCommandList() in WebGPU backend.
    void DILIGENT_CALL_TYPE FinishCommandList(ICommandList** ppCommandList) override;

    /// Implementation of IDeviceContext::ExecuteCommandLists() in WebGPU backend.
    void DILIGENT_CALL_TYPE ExecuteCommandLists(Uint32               NumCommandLists,
                                                ICommandList* const* ppCommandLists) override;

    /// Implementation of IDeviceContext::EnqueueSignal() in WebGPU backend.
    void DILIGENT_CALL_TYPE EnqueueSignal(IFence* pFence, Uint64 Value) override;

    /// Implementation of IDeviceContext::DeviceWaitForFence() in WebGPU backend.
    void DILIGENT_CALL_TYPE DeviceWaitForFence(IFence* pFence, Uint64 Value) override;

    /// Implementation of IDeviceContext::WaitForIdle() in WebGPU backend.
    void DILIGENT_CALL_TYPE WaitForIdle() override;

    /// Implementation of IDeviceContext::BeginQuery() in WebGPU backend.
    void DILIGENT_CALL_TYPE BeginQuery(IQuery* pQuery) override;

    /// Implementation of IDeviceContext::EndQuery() in WebGPU backend.
    void DILIGENT_CALL_TYPE EndQuery(IQuery* pQuery) override;

    /// Implementation of IDeviceContext::Flush() in WebGPU backend.
    void DILIGENT_CALL_TYPE Flush() override;

    /// Implementation of IDeviceContext::BuildBLAS() in WebGPU backend.
    void DILIGENT_CALL_TYPE BuildBLAS(const BuildBLASAttribs& Attribs) override;

    /// Implementation of IDeviceContext::BuildTLAS() in WebGPU backend.
    void DILIGENT_CALL_TYPE BuildTLAS(const BuildTLASAttribs& Attribs) override;

    /// Implementation of IDeviceContext::CopyBLAS() in WebGPU backend.
    void DILIGENT_CALL_TYPE CopyBLAS(const CopyBLASAttribs& Attribs) override;

    /// Implementation of IDeviceContext::CopyTLAS() in WebGPU backend.
    void DILIGENT_CALL_TYPE CopyTLAS(const CopyTLASAttribs& Attribs) override;

    /// Implementation of IDeviceContext::WriteBLASCompactedSize() in WebGPU backend.
    void DILIGENT_CALL_TYPE WriteBLASCompactedSize(const WriteBLASCompactedSizeAttribs& Attribs) override;

    /// Implementation of IDeviceContext::WriteTLASCompactedSize() in WebGPU backend.
    void DILIGENT_CALL_TYPE WriteTLASCompactedSize(const WriteTLASCompactedSizeAttribs& Attribs) override;

    /// Implementation of IDeviceContext::TraceRays() in WebGPU backend.
    void DILIGENT_CALL_TYPE TraceRays(const TraceRaysAttribs& Attribs) override;

    /// Implementation of IDeviceContext::TraceRaysIndirect() in WebGPU backend.
    void DILIGENT_CALL_TYPE TraceRaysIndirect(const TraceRaysIndirectAttribs& Attribs) override;

    /// Implementation of IDeviceContext::UpdateSBT() in WebGPU backend.
    void DILIGENT_CALL_TYPE UpdateSBT(IShaderBindingTable* pSBT, const UpdateIndirectRTBufferAttribs* pUpdateIndirectBufferAttribs) override;

    /// Implementation of IDeviceContext::BeginDebugGroup() in WebGPU backend.
    void DILIGENT_CALL_TYPE BeginDebugGroup(const Char* Name, const float* pColor) override;

    /// Implementation of IDeviceContext::EndDebugGroup() in WebGPU backend.
    void DILIGENT_CALL_TYPE EndDebugGroup() override;

    /// Implementation of IDeviceContext::InsertDebugLabel() in WebGPU backend.
    void DILIGENT_CALL_TYPE InsertDebugLabel(const Char* Label, const float* pColor) override;

    /// Implementation of IDeviceContext::SetShadingRate() in WebGPU backend.
    void DILIGENT_CALL_TYPE SetShadingRate(SHADING_RATE          BaseRate,
                                           SHADING_RATE_COMBINER PrimitiveCombiner,
                                           SHADING_RATE_COMBINER TextureCombiner) override;

    /// Implementation of IDeviceContext::BindSparseResourceMemory() in WebGPU backend.
    void DILIGENT_CALL_TYPE BindSparseResourceMemory(const BindSparseResourceMemoryAttribs& Attribs) override;

    /// Implementation of IDeviceContext::GenerateMips() in WebGPU backend.
    void DILIGENT_CALL_TYPE GenerateMips(ITextureView* pTexView) override;

    void DILIGENT_CALL_TYPE FinishFrame() override;

    void DILIGENT_CALL_TYPE TransitionResourceStates(Uint32 BarrierCount, const StateTransitionDesc* pResourceBarriers) override;

    ICommandQueue* DILIGENT_CALL_TYPE LockCommandQueue() override;

    void DILIGENT_CALL_TYPE UnlockCommandQueue() override;

    void DILIGENT_CALL_TYPE ResolveTextureSubresource(ITexture*                               pSrcTexture,
                                                      ITexture*                               pDstTexture,
                                                      const ResolveTextureSubresourceAttribs& ResolveAttribs) override;
    WGPUQueue DILIGENT_CALL_TYPE GetWebGPUQueue() override;

private:
};


} // namespace Diligent
