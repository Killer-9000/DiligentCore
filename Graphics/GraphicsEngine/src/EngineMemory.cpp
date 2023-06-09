/*
 *  Copyright 2019-2022 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
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

// stdafx.cpp : source file that includes just the standard includes
// RenderEngine.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "EngineMemory.h"
#include "DefaultRawMemoryAllocator.hpp"
#include "DebugUtilities.hpp"

namespace Diligent
{

static IMemoryAllocator* g_pRawAllocator;

void SetRawAllocator(IMemoryAllocator* pRawAllocator)
{
    if (pRawAllocator == nullptr)
    {
        LOG_INFO_MESSAGE("User-defined allocator is not provided. Using default allocator.");
        pRawAllocator = &DefaultRawMemoryAllocator::GetAllocator();
    }

    DEV_CHECK_ERR(g_pRawAllocator == nullptr || g_pRawAllocator == pRawAllocator,
                  "User-defined allocator has already been provided and does not match the new allocator. "
                  "This may result in undefined behavior.");

    g_pRawAllocator = pRawAllocator;
}

IMemoryAllocator& GetRawAllocator()
{
    return g_pRawAllocator != nullptr ? *g_pRawAllocator : DefaultRawMemoryAllocator::GetAllocator();
}

IMemoryAllocator& GetStringAllocator()
{
    return GetRawAllocator();
}

} // namespace Diligent
#if 0

void* operator new(size_t Size)
{ 
    return Diligent::GetRawAllocator().Allocate(Size, "<Unknown>", "<Unknown>", -1); 
}

void* operator new[](size_t Size) 
{ 
    return Diligent::GetRawAllocator().Allocate(Size, "<Unknown>", "<Unknown>", -1); 
}

void operator delete(void* Ptr)
{ 
    Diligent::GetRawAllocator().Free(Ptr); 
}

void operator delete[](void* Ptr)
{ 
    Diligent::GetRawAllocator().Free(Ptr); 
}
#endif
