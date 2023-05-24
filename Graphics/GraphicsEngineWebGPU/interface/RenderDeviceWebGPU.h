/*
 *  Copyright 2019-2023 Diligent Graphics LLC
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

#pragma once

/// \file
/// Definition of the Diligent::IRenderDeviceWebGPU interface

//TODO
#include <webgpu/webgpu.h>

#include "../../GraphicsEngine/interface/RenderDevice.h"

DILIGENT_BEGIN_NAMESPACE(Diligent)

// {BB1F1488-C10D-493F-8139-3B9010598B16}
static const INTERFACE_ID IID_RenderDeviceWebGPU =
    {0xBB1F1488, 0xC10D, 0x493F, {0x81, 0x39, 0x3B, 0x90, 0x10, 0x59, 0x8B, 0x16}};

#define DILIGENT_INTERFACE_NAME IRenderDeviceWebGPU
#include "../../../Primitives/interface/DefineInterfaceHelperMacros.h"

#define IRenderDeviceWebGPUInclusiveMethods \
    IRenderDeviceInclusiveMethods;          \
    IRenderDeviceWebGPUMethods RenderDeviceWebGPU

// clang-format off

/// Exposes WebGPU-specific functionality of a render device.
DILIGENT_BEGIN_INTERFACE(IRenderDeviceWebGPU, IRenderDevice)
{
    /// Returns WebGPU instance
    VIRTUAL WGPUInstance METHOD(GetWebGPUInstance)(THIS) PURE;

    /// Returns WebGPU adapter
    VIRTUAL WGPUAdapter METHOD(GetWebGPUAdapter)(THIS) PURE;

    /// Returns WebGPU device
    VIRTUAL WGPUDevice METHOD(GetWebGPUDevice)(THIS) PURE;

};
DILIGENT_END_INTERFACE

#include "../../../Primitives/interface/UndefInterfaceHelperMacros.h"

#if DILIGENT_C_INTERFACE

// clang-format off

//#    define IRenderDeviceWebGPU_GetWebGPUDevice(This)  CALL_IFACE_METHOD(RenderDeviceWebGPU, GetWebGPUDevice, This)

// clang-format on

#endif

DILIGENT_END_NAMESPACE // namespace Diligent
