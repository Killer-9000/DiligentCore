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

#include <memory>

#include <webgpu/wgpu.h>

namespace Diligent
{

template <typename WebGPUObjectType, typename WebGPUObjectDeleter>
class WebGPUObjectWrapper
{
public:
    WebGPUObjectWrapper() :
        m_ObjectHandle{}, m_ObjectDeleter{} {}

    ~WebGPUObjectWrapper()
    {
        if (m_ObjectHandle)
            m_ObjectDeleter(m_ObjectHandle);
    }

    explicit WebGPUObjectWrapper(WebGPUObjectType ObjectHandle, WebGPUObjectDeleter ObjectDeleter = WebGPUObjectDeleter{}) :
        m_ObjectHandle(ObjectHandle), m_ObjectDeleter(ObjectDeleter) {}

    WebGPUObjectWrapper(const WebGPUObjectWrapper&) = delete;

    WebGPUObjectWrapper& operator=(const WebGPUObjectWrapper&) = delete;

    WebGPUObjectWrapper(WebGPUObjectWrapper&& RHS) noexcept :
        m_ObjectHandle(RHS.Release()), m_ObjectDeleter(std::move(RHS.m_ObjectDeleter))
    {
    }

    WebGPUObjectWrapper& operator=(WebGPUObjectWrapper&& RHS) noexcept
    {
        Reset(RHS.Release());
        m_ObjectDeleter = std::move(RHS.m_ObjectDeleter);
        return *this;
    }

    WebGPUObjectType Get() const
    {
        return m_ObjectHandle;
    }

    void Reset(WebGPUObjectType Handle)
    {
        if (m_ObjectHandle != Handle)
        {
            if (m_ObjectHandle)
                m_ObjectDeleter(m_ObjectHandle);
            m_ObjectHandle = Handle;
        }
    }

    WebGPUObjectType Release()
    {
        WebGPUObjectType ReleaseHandle = m_ObjectHandle;
        m_ObjectHandle                 = nullptr;
        return ReleaseHandle;
    }

    explicit operator bool() const
    {
        return m_ObjectHandle != nullptr;
    }

private:
    WebGPUObjectType    m_ObjectHandle;
    WebGPUObjectDeleter m_ObjectDeleter;
};

#ifdef PLATFORM_EMSCRIPTEN
#    define DECLARE_WEBGPU_WRAPPER(HandleName, TypeName, FunctionName) \
        struct HandleName##Deleter                                     \
        {                                                              \
            void operator()(TypeName Handle) const                     \
            {                                                          \
            }                                                          \
        };                                                             \
        using HandleName##Wrapper = WebGPUObjectWrapper<TypeName, HandleName##Deleter>;

#else
#    define DECLARE_WEBGPU_WRAPPER(HandleName, TypeName, FunctionName) \
        struct HandleName##Deleter                                     \
        {                                                              \
            void operator()(TypeName Handle) const                     \
            {                                                          \
                FunctionName(Handle);                                  \
            }                                                          \
        };                                                             \
        using HandleName##Wrapper = WebGPUObjectWrapper<TypeName, HandleName##Deleter>;
#endif

// TODO: TextureDestroy, BufferDestroy, QuerySetDestroy, DeviceDestroy

DECLARE_WEBGPU_WRAPPER(WebGPUInstance, WGPUInstance, wgpuInstanceDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUAdapter, WGPUAdapter, wgpuAdapterDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUDevice, WGPUDevice, wgpuDeviceDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUSwapChain, WGPUSwapChain, wgpuSwapChainDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUSurface, WGPUSurface, wgpuSurfaceDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUTexture, WGPUTexture, wgpuTextureDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUTextureView, WGPUTextureView, wgpuTextureViewDrop)
DECLARE_WEBGPU_WRAPPER(WebGPUBuffer, WGPUBuffer, wgpuBufferDestroy);

} // namespace Diligent
