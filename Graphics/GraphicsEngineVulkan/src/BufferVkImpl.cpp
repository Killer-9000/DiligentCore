/*     Copyright 2015-2018 Egor Yusov
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF ANY PROPRIETARY RIGHTS.
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

#include "pch.h"
#include "BufferVkImpl.h"
#include "RenderDeviceVkImpl.h"
#include "DeviceContextVkImpl.h"
#include "VulkanTypeConversions.h"
#include "BufferViewVkImpl.h"
#include "GraphicsAccessories.h"
#include "EngineMemory.h"
#include "StringTools.h"
#include "VulkanUtilities/VulkanDebug.h"

namespace Diligent
{

BufferVkImpl :: BufferVkImpl(IReferenceCounters *pRefCounters, 
                             FixedBlockMemoryAllocator &BuffViewObjMemAllocator, 
                             RenderDeviceVkImpl *pRenderDeviceVk, 
                             const BufferDesc& BuffDesc, 
                             const BufferData &BuffData /*= BufferData()*/) : 
    TBufferBase(pRefCounters, BuffViewObjMemAllocator, pRenderDeviceVk, BuffDesc, false)/*,
#ifdef _DEBUG
    m_DbgMapType(1 + pRenderDeviceVk->GetNumDeferredContexts(), std::make_pair(static_cast<MAP_TYPE>(-1), static_cast<Uint32>(-1)), STD_ALLOCATOR_RAW_MEM(DynamicAllocation, GetRawAllocator(), "Allocator for vector<pair<MAP_TYPE,Uint32>>")),
#endif
    m_DynamicData(BuffDesc.Usage == USAGE_DYNAMIC ? (1 + pRenderDeviceVk->GetNumDeferredContexts()) : 0, DynamicAllocation(), STD_ALLOCATOR_RAW_MEM(DynamicAllocation, GetRawAllocator(), "Allocator for vector<DynamicAllocation>"))
    */
{
#define LOG_BUFFER_ERROR_AND_THROW(...) LOG_ERROR_AND_THROW("Buffer \"", BuffDesc.Name ? BuffDesc.Name : "", "\": ", ##__VA_ARGS__);

    if( m_Desc.Usage == USAGE_STATIC && BuffData.pData == nullptr )
        LOG_BUFFER_ERROR_AND_THROW("Static buffer must be initialized with data at creation time")

    if( m_Desc.Usage == USAGE_DYNAMIC && BuffData.pData != nullptr )
        LOG_BUFFER_ERROR_AND_THROW("Dynamic buffer must be initialized via Map()")

    if (m_Desc.Usage == USAGE_CPU_ACCESSIBLE)
    {
        if (m_Desc.CPUAccessFlags != CPU_ACCESS_WRITE && m_Desc.CPUAccessFlags != CPU_ACCESS_READ)
            LOG_BUFFER_ERROR_AND_THROW("Exactly one of the CPU_ACCESS_WRITE or CPU_ACCESS_READ flags must be specified for a cpu-accessible buffer")

        if (m_Desc.CPUAccessFlags == CPU_ACCESS_WRITE)
        {
            if(BuffData.pData != nullptr )
                LOG_BUFFER_ERROR_AND_THROW("CPU-writable staging buffers must be updated via map")
        }
    }

    auto vkAllocator = pRenderDeviceVk->GetVulkanInstance()->GetVkAllocator();
    if(m_Desc.Usage == USAGE_DYNAMIC && (m_Desc.BindFlags & (BIND_SHADER_RESOURCE|BIND_UNORDERED_ACCESS)) == 0)
    {
        UNSUPPORTED("Dynamic buffers are not yet implemented");
        // Dynamic constant/vertex/index buffers are suballocated in the upload heap when Map() is called.
        // Dynamic buffers with SRV or UAV flags need to be allocated in GPU-only memory
        // Dynamic upload heap buffer is always in Vk_RESOURCE_STATE_GENERIC_READ state
#if 0
        m_UsageState = Vk_RESOURCE_STATE_GENERIC_READ;
        VERIFY_EXPR(m_DynamicData.size() == 1 + pRenderDeviceVk->GetNumDeferredContexts());
#endif
    }
    else
    {
        VkBufferCreateInfo VkBuffCI = {};
        VkBuffCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        VkBuffCI.pNext = nullptr;
        VkBuffCI.flags = 0; // VK_BUFFER_CREATE_SPARSE_BINDING_BIT, VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT, VK_BUFFER_CREATE_SPARSE_ALIASED_BIT
        VkBuffCI.size = m_Desc.uiSizeInBytes;
        VkBuffCI.usage =
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT | // The buffer can be used as the source of a transfer command 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT;  // The buffer can be used as the destination of a transfer command
        if (m_Desc.BindFlags & BIND_UNORDERED_ACCESS)
            VkBuffCI.usage |= m_Desc.Mode == BUFFER_MODE_FORMATTED ? VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (m_Desc.BindFlags & BIND_SHADER_RESOURCE)
            VkBuffCI.usage |= m_Desc.Mode == BUFFER_MODE_FORMATTED ? VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (m_Desc.BindFlags & BIND_VERTEX_BUFFER)
            VkBuffCI.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (m_Desc.BindFlags & BIND_INDEX_BUFFER)
            VkBuffCI.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (m_Desc.BindFlags & BIND_INDIRECT_DRAW_ARGS)
            VkBuffCI.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        if (m_Desc.BindFlags & BIND_UNIFORM_BUFFER)
            VkBuffCI.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        VkBuffCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // sharing mode of the buffer when it will be accessed by multiple queue families.
        VkBuffCI.queueFamilyIndexCount = 0; // number of entries in the pQueueFamilyIndices array
        VkBuffCI.pQueueFamilyIndices = nullptr; // list of queue families that will access this buffer 
                                                // (ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT).

        auto vkDevice = pRenderDeviceVk->GetVkDevice();
        auto err = vkCreateBuffer(vkDevice, &VkBuffCI, vkAllocator, &m_VkBuffer);
        CHECK_VK_ERROR_AND_THROW(err, "Failed to create Vulkan buffer object");

        VkMemoryRequirements MemReqs = {};
        vkGetBufferMemoryRequirements(vkDevice, m_VkBuffer, &MemReqs);

        VkMemoryAllocateInfo MemAlloc = {};
        MemAlloc.pNext = nullptr;
        MemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        MemAlloc.allocationSize = MemReqs.size;
        
        auto& PhysicalDevice = pRenderDeviceVk->GetPhysicalDevice();
       
        VkMemoryPropertyFlags BufferMemoryFlags = 0;
        if (m_Desc.Usage == USAGE_CPU_ACCESSIBLE)
            BufferMemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        else
            BufferMemoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        // memoryTypeBits is a bitmask and contains one bit set for every supported memory type for the resource. 
        // Bit i is set if and only if the memory type i in the VkPhysicalDeviceMemoryProperties structure for the 
        // physical device is supported for the resource.
        MemAlloc.memoryTypeIndex = PhysicalDevice.GetMemoryTypeIndex(MemReqs.memoryTypeBits, BufferMemoryFlags);
        if(BufferMemoryFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            // There must be at least one memory type with the DEVICE_LOCAL_BIT bit set
            VERIFY(MemAlloc.memoryTypeIndex != VulkanUtilities::VulkanPhysicalDevice::InvalidMemoryTypeIndex,
                   "Vulkan spec requires that memoryTypeBits member always contains "
                   "at least one bit set corresponding to a VkMemoryType with a propertyFlags that has the "
                   "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit set (11.6)");
        }
        else if(MemAlloc.memoryTypeIndex != VulkanUtilities::VulkanPhysicalDevice::InvalidMemoryTypeIndex)
        {
            LOG_ERROR_AND_THROW("Failed to find suitable device memory type for a buffer");
        }

        err = vkAllocateMemory(vkDevice, &MemAlloc, vkAllocator, &m_BufferMemory);
        CHECK_VK_ERROR_AND_THROW(err, "Failed to allocate device local memory for a Vulkan buffer object");

        err = vkBindBufferMemory(vkDevice, m_VkBuffer, m_BufferMemory, 0 /*offset*/);
        CHECK_VK_ERROR_AND_THROW(err, "Failed to bind buffer memory");

        bool bInitializeBuffer = (BuffData.pData != nullptr && BuffData.DataSize > 0);
        //if(bInitializeBuffer)
        //    m_UsageState = Vk_RESOURCE_STATE_COPY_DEST;

        if( *m_Desc.Name != 0)
            VulkanUtilities::SetBufferName(vkDevice, m_VkBuffer, m_Desc.Name);

	    if( bInitializeBuffer )
        {
            VkBufferCreateInfo VkStaginBuffCI = VkBuffCI;
            VkStaginBuffCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VkBuffer vkStagingBuffer = VK_NULL_HANDLE;
            err = vkCreateBuffer(vkDevice, &VkStaginBuffCI, vkAllocator, &vkStagingBuffer);
            CHECK_VK_ERROR_AND_THROW(err, "Failed to create staging buffer");

            VulkanUtilities::SetBufferName(vkDevice, vkStagingBuffer, "Staging buffer");

            VkMemoryRequirements StagingBufferMemReqs = {};
            vkGetBufferMemoryRequirements(vkDevice, vkStagingBuffer, &StagingBufferMemReqs);

            VkMemoryAllocateInfo StagingMemAlloc = {};
            StagingMemAlloc.pNext = nullptr;
            StagingMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            StagingMemAlloc.allocationSize = StagingBufferMemReqs.size;
            
            // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit specifies that the host cache management commands vkFlushMappedMemoryRanges 
            // and vkInvalidateMappedMemoryRanges are NOT needed to flush host writes to the device or make device writes visible
            // to the host (10.2)
            StagingMemAlloc.memoryTypeIndex = PhysicalDevice.GetMemoryTypeIndex(StagingBufferMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            VERIFY(StagingMemAlloc.memoryTypeIndex != VulkanUtilities::VulkanPhysicalDevice::InvalidMemoryTypeIndex,
                   "Vulkan spec requires that for a VkBuffer not created with the "
                   "VK_BUFFER_CREATE_SPARSE_BINDING_BIT bit set, the memoryTypeBits member always contains at least one bit set "
                   "corresponding to a VkMemoryType with a propertyFlags that has both the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit "
                   "and the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit set(11.6)");

            VkDeviceMemory vkStagingBufferMemory = VK_NULL_HANDLE;
            err = vkAllocateMemory(vkDevice, &StagingMemAlloc, vkAllocator, &vkStagingBufferMemory);
            CHECK_VK_ERROR_AND_THROW(err, "Failed to allocate memory for a staging buffer object");

            {
                void *StagingData = nullptr;
                err = vkMapMemory(vkDevice, vkStagingBufferMemory, 
                    0, // offset
                    StagingMemAlloc.allocationSize,
                    0, // flags, reserved for future use
                    &StagingData);
                CHECK_VK_ERROR_AND_THROW(err, "Failed to map staging memory");
                memcpy(StagingData, BuffData.pData, BuffData.DataSize);
                vkUnmapMemory(vkDevice, vkStagingBufferMemory);
            }
            
            err = vkBindBufferMemory(vkDevice, vkStagingBuffer, vkStagingBufferMemory, 0 /*offset*/);
            CHECK_VK_ERROR_AND_THROW(err, "Failed to bind staging bufer memory");
#if 0
            auto  *pInitContext = pRenderDeviceVk->AllocateCommandContext();
	        // copy data to the intermediate upload heap and then schedule a copy from the upload heap to the default buffer
            VERIFY_EXPR(m_UsageState == Vk_RESOURCE_STATE_COPY_DEST);
            // We MUST NOT call TransitionResource() from here, because
            // it will call AddRef() and potentially Release(), while 
            // the object is not constructed yet
	        pInitContext->CopyResource(m_pVkResource, UploadBuffer);

            // Command list fence should only be signaled when submitting cmd list
            // from the immediate context, otherwise the basic requirement will be violated
            // as in the scenario below
            // See http://diligentgraphics.com/diligent-engine/architecture/Vk/managing-resource-lifetimes/
            //                                                           
            //  Signaled Fence  |        Immediate Context               |            InitContext            |
            //                  |                                        |                                   |
            //    N             |  Draw(ResourceX)                       |                                   |
            //                  |  Release(ResourceX)                    |                                   |
            //                  |   - (ResourceX, N) -> Release Queue    |                                   |
            //                  |                                        | CopyResource()                    |
            //   N+1            |                                        | CloseAndExecuteCommandContext()   |
            //                  |                                        |                                   |
            //   N+2            |  CloseAndExecuteCommandContext()       |                                   |
            //                  |   - Cmd list is submitted with number  |                                   |
            //                  |     N+1, but resource it references    |                                   |
            //                  |     was added to the delete queue      |                                   |
            //                  |     with value N                       |                                   |
	        pRenderDeviceVk->CloseAndExecuteCommandContext(pInitContext, false);

            // Add reference to the object to the release queue to keep it alive
            // until copy operation is complete. This must be done after
            // submitting command list for execution!
            pRenderDeviceVk->SafeReleaseVkObject(UploadBuffer);
#endif
        }

        //if (m_Desc.BindFlags & BIND_UNIFORM_BUFFER)
        //{
        //    m_CBVDescriptorAllocation = pRenderDeviceVk->AllocateDescriptor(Vk_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        //    CreateCBV(m_CBVDescriptorAllocation.GetCpuHandle());
        //}
    }
}


static BufferDesc BufferDescFromVkResource(BufferDesc BuffDesc, void *pVkBuffer)
{
#if 0
    VERIFY(BuffDesc.Usage != USAGE_DYNAMIC, "Dynamic buffers cannot be attached to native Vk resource");

    auto VkBuffDesc = pVkBuffer->GetDesc();
    VERIFY(VkBuffDesc.Dimension == Vk_RESOURCE_DIMENSION_BUFFER, "Vk resource is not a buffer");

    VERIFY(BuffDesc.uiSizeInBytes == 0 || BuffDesc.uiSizeInBytes == VkBuffDesc.Width, "Buffer size specified by the BufferDesc (", BuffDesc.uiSizeInBytes,") does not match Vk resource size (", VkBuffDesc.Width, ")" );
    BuffDesc.uiSizeInBytes = static_cast<Uint32>( VkBuffDesc.Width );

    if (VkBuffDesc.Flags & Vk_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
    {
        VERIFY(BuffDesc.BindFlags == 0 || (BuffDesc.BindFlags & BIND_UNORDERED_ACCESS), "BIND_UNORDERED_ACCESS flag is not specified by the BufferDesc, while Vk resource was created with Vk_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS flag");
        BuffDesc.BindFlags |= BIND_UNORDERED_ACCESS;
    }
    if (VkBuffDesc.Flags & Vk_RESOURCE_FLAG_DENY_SHADER_RESOURCE)
    {
        VERIFY( !(BuffDesc.BindFlags & BIND_SHADER_RESOURCE), "BIND_SHADER_RESOURCE flag is specified by the BufferDesc, while Vk resource was created with Vk_RESOURCE_FLAG_DENY_SHADER_RESOURCE flag");
        BuffDesc.BindFlags &= ~BIND_SHADER_RESOURCE;
    }

    if( (BuffDesc.BindFlags & BIND_UNORDERED_ACCESS) || (BuffDesc.BindFlags & BIND_SHADER_RESOURCE) )
    {
        if(BuffDesc.Mode == BUFFER_MODE_STRUCTURED)
        {
            VERIFY(BuffDesc.ElementByteStride != 0, "Element byte stride cannot be 0 for a structured buffer");
        }
        else if(BuffDesc.Mode == BUFFER_MODE_FORMATTED)
        {
            VERIFY( BuffDesc.Format.ValueType != VT_UNDEFINED, "Value type is not specified for a formatted buffer" );
            VERIFY( BuffDesc.Format.NumComponents != 0, "Num components cannot be zero in a formated buffer" );
        }
        else
        {
            UNEXPECTED("Buffer mode must be structured or formatted");
        }
    }
#endif
    return BuffDesc;
}

BufferVkImpl :: BufferVkImpl(IReferenceCounters *pRefCounters, 
                                   FixedBlockMemoryAllocator &BuffViewObjMemAllocator, 
                                   RenderDeviceVkImpl *pRenderDeviceVk, 
                                   const BufferDesc& BuffDesc,
                                   void *pVkBuffer) : 
    TBufferBase(pRefCounters, BuffViewObjMemAllocator, pRenderDeviceVk, BufferDescFromVkResource(BuffDesc, pVkBuffer), false)/*,
#ifdef _DEBUG
    m_DbgMapType(1 + pRenderDeviceVk->GetNumDeferredContexts(), std::make_pair(static_cast<MAP_TYPE>(-1), static_cast<Uint32>(-1)), STD_ALLOCATOR_RAW_MEM(DynamicAllocation, GetRawAllocator(), "Allocator for vector<pair<MAP_TYPE,Uint32>>")),
#endif
    m_DynamicData(BuffDesc.Usage == USAGE_DYNAMIC ? (1 + pRenderDeviceVk->GetNumDeferredContexts()) : 0, DynamicAllocation(), STD_ALLOCATOR_RAW_MEM(DynamicAllocation, GetRawAllocator(), "Allocator for vector<DynamicAllocation>"))
    */
{
#if 0
    m_pVkResource = pVkBuffer;

    if (m_Desc.BindFlags & BIND_UNIFORM_BUFFER)
    {
        m_CBVDescriptorAllocation = pRenderDeviceVk->AllocateDescriptor(Vk_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        CreateCBV(m_CBVDescriptorAllocation.GetCpuHandle());
    }
#endif
}
BufferVkImpl :: ~BufferVkImpl()
{
    // Vk object can only be destroyed when it is no longer used by the GPU
    if(m_VkBuffer!=VK_NULL_HANDLE)
    {
        auto *pDeviceVkImpl = ValidatedCast<RenderDeviceVkImpl>(GetDevice());
        pDeviceVkImpl->SafeReleaseVkBuffer(m_VkBuffer);
    }
}

IMPLEMENT_QUERY_INTERFACE( BufferVkImpl, IID_BufferVk, TBufferBase )

void BufferVkImpl::UpdateData( IDeviceContext *pContext, Uint32 Offset, Uint32 Size, const PVoid pData )
{
    TBufferBase::UpdateData( pContext, Offset, Size, pData );

#if 0
    // We must use cmd context from the device context provided, otherwise there will
    // be resource barrier issues in the cmd list in the device context
    auto *pDeviceContextVk = ValidatedCast<DeviceContextVkImpl>(pContext);
    pDeviceContextVk->UpdateBufferRegion(this, pData, Offset, Size);
#endif
}

void BufferVkImpl :: CopyData(IDeviceContext *pContext, IBuffer *pSrcBuffer, Uint32 SrcOffset, Uint32 DstOffset, Uint32 Size)
{
#if 0
    TBufferBase::CopyData( pContext, pSrcBuffer, SrcOffset, DstOffset, Size );
    auto *pDeviceContextVk = ValidatedCast<DeviceContextVkImpl>(pContext);
    pDeviceContextVk->CopyBufferRegion(ValidatedCast<BufferVkImpl>(pSrcBuffer), this, SrcOffset, DstOffset, Size);
#endif
}

void BufferVkImpl :: Map(IDeviceContext *pContext, MAP_TYPE MapType, Uint32 MapFlags, PVoid &pMappedData)
{
    TBufferBase::Map( pContext, MapType, MapFlags, pMappedData );
#if 0
    auto *pDeviceContextVk = ValidatedCast<DeviceContextVkImpl>(pContext);
#ifdef _DEBUG
    if(pDeviceContextVk != nullptr)
        m_DbgMapType[pDeviceContextVk->GetContextId()] = std::make_pair(MapType, MapFlags);
#endif
    if (MapType == MAP_READ )
    {
        LOG_WARNING_MESSAGE_ONCE("Mapping CPU buffer for reading on Vk currently requires flushing context and idling GPU");
        pDeviceContextVk->Flush();
        auto *pDeviceVk = ValidatedCast<RenderDeviceVkImpl>(GetDevice());
        pDeviceVk->IdleGPU(false);

        VERIFY(m_Desc.Usage == USAGE_CPU_ACCESSIBLE, "Buffer must be created as USAGE_CPU_ACCESSIBLE to be mapped for reading");
        Vk_RANGE MapRange;
        MapRange.Begin = 0;
        MapRange.End = m_Desc.uiSizeInBytes;
        m_pVkResource->Map(0, &MapRange, &pMappedData);
    }
    else if(MapType == MAP_WRITE)
    {
        if (m_Desc.Usage == USAGE_CPU_ACCESSIBLE)
        {
            VERIFY(m_pVkResource != nullptr, "USAGE_CPU_ACCESSIBLE buffer mapped for writing must intialize Vk resource");
            if (MapFlags & MAP_FLAG_DISCARD)
            {
                
            }
            m_pVkResource->Map(0, nullptr, &pMappedData);
        }
        else if (m_Desc.Usage == USAGE_DYNAMIC)
        {
            VERIFY(MapFlags & MAP_FLAG_DISCARD, "Vk buffer must be mapped for writing with MAP_FLAG_DISCARD flag");
            auto *pCtxVk = ValidatedCast<DeviceContextVkImpl>(pContext);
            auto ContextId = pDeviceContextVk->GetContextId();
            m_DynamicData[ContextId] = pCtxVk->AllocateDynamicSpace(m_Desc.uiSizeInBytes);
            pMappedData = m_DynamicData[ContextId].CPUAddress;
        }
        else
        {
            LOG_ERROR("Only USAGE_DYNAMIC and USAGE_CPU_ACCESSIBLE Vk buffers can be mapped for writing");
        }
    }
    else if(MapType == MAP_READ_WRITE)
    {
        LOG_ERROR("MAP_READ_WRITE is not supported on Vk");
    }
    else
    {
        LOG_ERROR("Only MAP_WRITE_DISCARD and MAP_READ are currently implemented in Vk");
    }
#endif
}

void BufferVkImpl::Unmap( IDeviceContext *pContext, MAP_TYPE MapType, Uint32 MapFlags )
{
    TBufferBase::Unmap( pContext, MapType, MapFlags );

#if 0
    auto *pDeviceContextVk = ValidatedCast<DeviceContextVkImpl>(pContext);
    Uint32 CtxId = pDeviceContextVk != nullptr ? pDeviceContextVk->GetContextId() : static_cast<Uint32>(-1);
#ifdef _DEBUG
    if (pDeviceContextVk != nullptr)
    {
        VERIFY(m_DbgMapType[CtxId].first == MapType, "Map type does not match the type provided to Map()");
        VERIFY(m_DbgMapType[CtxId].second == MapFlags, "Map flags do not match the flags provided to Map()");
    }
#endif

    if (MapType == MAP_READ )
    {
        Vk_RANGE MapRange;
        // It is valid to specify the CPU didn't write any data by passing a range where End is less than or equal to Begin.
        MapRange.Begin = 1;
        MapRange.End = 0;
        m_pVkResource->Unmap(0, &MapRange);
    }
    else if(MapType == MAP_WRITE)
    {
        if (m_Desc.Usage == USAGE_CPU_ACCESSIBLE)
        {
            VERIFY(m_pVkResource != nullptr, "USAGE_CPU_ACCESSIBLE buffer mapped for writing must intialize Vk resource");
            m_pVkResource->Unmap(0, nullptr);
        }
        else if (m_Desc.Usage == USAGE_DYNAMIC)
        {
            VERIFY(MapFlags & MAP_FLAG_DISCARD, "Vk buffer must be mapped for writing with MAP_FLAG_DISCARD flag");
            // Copy data into the resource
            if (m_pVkResource)
            {
                pDeviceContextVk->UpdateBufferRegion(this, m_DynamicData[CtxId], 0, m_Desc.uiSizeInBytes);
            }
        }
    }

#ifdef _DEBUG
    if(pDeviceContextVk != nullptr)
        m_DbgMapType[CtxId] = std::make_pair(static_cast<MAP_TYPE>(-1), static_cast<Uint32>(-1));
#endif
#endif
}

void BufferVkImpl::CreateViewInternal( const BufferViewDesc &OrigViewDesc, IBufferView **ppView, bool bIsDefaultView )
{
    VERIFY( ppView != nullptr, "Null pointer provided" );
    if( !ppView )return;
    VERIFY( *ppView == nullptr, "Overwriting reference to existing object may cause memory leaks" );

    *ppView = nullptr;

    try
    {
        auto *pDeviceVkImpl = ValidatedCast<RenderDeviceVkImpl>(GetDevice());
        auto &BuffViewAllocator = pDeviceVkImpl->GetBuffViewObjAllocator();
        VERIFY( &BuffViewAllocator == &m_dbgBuffViewAllocator, "Buff view allocator does not match allocator provided at buffer initialization" );

        BufferViewDesc ViewDesc = OrigViewDesc;
        if( ViewDesc.ViewType == BUFFER_VIEW_UNORDERED_ACCESS || ViewDesc.ViewType == BUFFER_VIEW_SHADER_RESOURCE )
        {
            auto vkView = CreateView(ViewDesc);
            *ppView = NEW_RC_OBJ(BuffViewAllocator, "BufferViewVkImpl instance", BufferViewVkImpl, bIsDefaultView ? this : nullptr)
                                (GetDevice(), ViewDesc, this, vkView, bIsDefaultView );
        }

        if( !bIsDefaultView && *ppView )
            (*ppView)->AddRef();
    }
    catch( const std::runtime_error & )
    {
        const auto *ViewTypeName = GetBufferViewTypeLiteralName(OrigViewDesc.ViewType);
        LOG_ERROR("Failed to create view \"", OrigViewDesc.Name ? OrigViewDesc.Name : "", "\" (", ViewTypeName, ") for buffer \"", m_Desc.Name, "\"" );
    }
}


VkBufferView BufferVkImpl::CreateView(struct BufferViewDesc &ViewDesc)
{
    VkBufferView vkBuffView = VK_NULL_HANDLE;
    CorrectBufferViewDesc(ViewDesc);
    if( (ViewDesc.ViewType == BUFFER_VIEW_SHADER_RESOURCE || ViewDesc.ViewType == BUFFER_VIEW_UNORDERED_ACCESS) && 
        m_Desc.Mode == BUFFER_MODE_FORMATTED)
    {
        VkBufferViewCreateInfo ViewCI = {};
        ViewCI.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        ViewCI.pNext = nullptr;
        ViewCI.flags = 0; // reserved for future use
        ViewCI.buffer = m_VkBuffer;
        ViewCI.format = TypeToVkFormat(m_Desc.Format.ValueType, m_Desc.Format.NumComponents, m_Desc.Format.IsNormalized);
        ViewCI.offset = ViewDesc.ByteOffset;
        ViewCI.range = ViewDesc.ByteWidth; // size in bytes of the buffer view

        auto *pDeviceVkImpl = static_cast<RenderDeviceVkImpl*>(GetDevice());
        auto vkAllocator = pDeviceVkImpl->GetVulkanInstance()->GetVkAllocator();
        auto vkDevice = pDeviceVkImpl->GetVkDevice();
        auto err = vkCreateBufferView(vkDevice, &ViewCI, vkAllocator, &vkBuffView);
        CHECK_VK_ERROR_AND_THROW(err, "Failed to create Vulkan buffer view");
    }
    return vkBuffView;
}

#if 0
#ifdef _DEBUG
void BufferVkImpl::DbgVerifyDynamicAllocation(Uint32 ContextId)
{
    VERIFY(m_DynamicData[ContextId].GPUAddress != 0, "Dynamic buffer must be mapped before the first use");
	auto CurrentFrame = ValidatedCast<RenderDeviceVkImpl>(GetDevice())->GetCurrentFrameNumber();
    VERIFY(m_DynamicData[ContextId].FrameNum == CurrentFrame, "Dynamic allocation is out-of-date. Dynamic buffer \"", m_Desc.Name, "\" must be mapped in the same frame it is used.");
    VERIFY(GetState() == Vk_RESOURCE_STATE_GENERIC_READ, "Dynamic buffers are expected to always be in Vk_RESOURCE_STATE_GENERIC_READ state");
}
#endif

#endif
}
