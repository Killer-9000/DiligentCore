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

#include "WebGPUTypeConversions.hpp"

#include "DebugUtilities.hpp"

namespace Diligent
{

class TexFormatToWebGPUFormatMapper
{
public:
    TexFormatToWebGPUFormatMapper()
    {
        // clang-format off
        m_FmtToWGPUFmtMap[TEX_FORMAT_UNKNOWN] = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA32_TYPELESS] = WGPUTextureFormat_RGBA32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA32_FLOAT]    = WGPUTextureFormat_RGBA32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA32_UINT]     = WGPUTextureFormat_RGBA32Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA32_SINT]     = WGPUTextureFormat_RGBA32Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB32_TYPELESS] = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB32_FLOAT]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB32_UINT]     = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB32_SINT]     = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA16_TYPELESS] = WGPUTextureFormat_RGBA16Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA16_FLOAT]    = WGPUTextureFormat_RGBA16Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA16_UNORM]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA16_UINT]     = WGPUTextureFormat_RGBA16Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA16_SNORM]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA16_SINT]     = WGPUTextureFormat_RGBA16Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RG32_TYPELESS] = WGPUTextureFormat_RG32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG32_FLOAT]    = WGPUTextureFormat_RG32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG32_UINT]     = WGPUTextureFormat_RG32Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG32_SINT]     = WGPUTextureFormat_RG32Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_R32G8X24_TYPELESS]        = WGPUTextureFormat_Depth32FloatStencil8;
        m_FmtToWGPUFmtMap[TEX_FORMAT_D32_FLOAT_S8X24_UINT]     = WGPUTextureFormat_Depth32FloatStencil8;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R32_FLOAT_X8X24_TYPELESS] = WGPUTextureFormat_Depth32FloatStencil8;
        m_FmtToWGPUFmtMap[TEX_FORMAT_X32_TYPELESS_G8X24_UINT]  = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB10A2_TYPELESS] = WGPUTextureFormat_RGB10A2Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB10A2_UNORM]    = WGPUTextureFormat_RGB10A2Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB10A2_UINT]     = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R11G11B10_FLOAT]  = WGPUTextureFormat_RG11B10Ufloat;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA8_TYPELESS]   = WGPUTextureFormat_RGBA8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA8_UNORM]      = WGPUTextureFormat_RGBA8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA8_UNORM_SRGB] = WGPUTextureFormat_RGBA8UnormSrgb;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA8_UINT]       = WGPUTextureFormat_RGBA8Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA8_SNORM]      = WGPUTextureFormat_RGBA8Snorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RGBA8_SINT]       = WGPUTextureFormat_RGBA8Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RG16_TYPELESS] = WGPUTextureFormat_RG16Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG16_FLOAT]    = WGPUTextureFormat_RG16Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG16_UNORM]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG16_UINT]     = WGPUTextureFormat_RG16Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG16_SNORM]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG16_SINT]     = WGPUTextureFormat_RG16Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_R32_TYPELESS] = WGPUTextureFormat_R32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_D32_FLOAT]    = WGPUTextureFormat_Depth32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R32_FLOAT]    = WGPUTextureFormat_R32Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R32_UINT]     = WGPUTextureFormat_R32Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R32_SINT]     = WGPUTextureFormat_R32Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_R24G8_TYPELESS]        = WGPUTextureFormat_Depth24PlusStencil8;
        m_FmtToWGPUFmtMap[TEX_FORMAT_D24_UNORM_S8_UINT]     = WGPUTextureFormat_Depth24PlusStencil8;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R24_UNORM_X8_TYPELESS] = WGPUTextureFormat_Depth24PlusStencil8;
        m_FmtToWGPUFmtMap[TEX_FORMAT_X24_TYPELESS_G8_UINT]  = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RG8_TYPELESS] = WGPUTextureFormat_RG8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG8_UNORM]    = WGPUTextureFormat_RG8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG8_UINT]     = WGPUTextureFormat_RG8Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG8_SNORM]    = WGPUTextureFormat_RG8Snorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG8_SINT]     = WGPUTextureFormat_RG8Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_R16_TYPELESS] = WGPUTextureFormat_R16Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R16_FLOAT]    = WGPUTextureFormat_R16Float;
        m_FmtToWGPUFmtMap[TEX_FORMAT_D16_UNORM]    = WGPUTextureFormat_Depth16Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R16_UNORM]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R16_UINT]     = WGPUTextureFormat_R16Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R16_SNORM]    = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R16_SINT]     = WGPUTextureFormat_R16Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_R8_TYPELESS] = WGPUTextureFormat_R8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R8_UNORM]    = WGPUTextureFormat_R8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R8_UINT]     = WGPUTextureFormat_R8Uint;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R8_SNORM]    = WGPUTextureFormat_R8Snorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R8_SINT]     = WGPUTextureFormat_R8Sint;

        m_FmtToWGPUFmtMap[TEX_FORMAT_A8_UNORM]    = WGPUTextureFormat_R8Unorm;

        m_FmtToWGPUFmtMap[TEX_FORMAT_R1_UNORM]    = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_RGB9E5_SHAREDEXP] = WGPUTextureFormat_RGB9E5Ufloat;
        m_FmtToWGPUFmtMap[TEX_FORMAT_RG8_B8G8_UNORM]   = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_G8R8_G8B8_UNORM]  = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_BC1_TYPELESS]   = WGPUTextureFormat_BC1RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC1_UNORM]      = WGPUTextureFormat_BC1RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC1_UNORM_SRGB] = WGPUTextureFormat_BC1RGBAUnormSrgb;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC2_TYPELESS]   = WGPUTextureFormat_BC2RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC2_UNORM]      = WGPUTextureFormat_BC2RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC2_UNORM_SRGB] = WGPUTextureFormat_BC2RGBAUnormSrgb;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC3_TYPELESS]   = WGPUTextureFormat_BC3RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC3_UNORM]      = WGPUTextureFormat_BC3RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC3_UNORM_SRGB] = WGPUTextureFormat_BC3RGBAUnormSrgb;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC4_TYPELESS]   = WGPUTextureFormat_BC4RUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC4_UNORM]      = WGPUTextureFormat_BC4RUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC4_SNORM]      = WGPUTextureFormat_BC4RSnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC5_TYPELESS]   = WGPUTextureFormat_BC5RGUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC5_UNORM]      = WGPUTextureFormat_BC5RGUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC5_SNORM]      = WGPUTextureFormat_BC5RGSnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_B5G6R5_UNORM]   = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_B5G5R5A1_UNORM] = WGPUTextureFormat_Undefined;

        m_FmtToWGPUFmtMap[TEX_FORMAT_BGRA8_UNORM]          = WGPUTextureFormat_BGRA8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BGRX8_UNORM]          = WGPUTextureFormat_BGRA8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM] = WGPUTextureFormat_Undefined;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BGRA8_TYPELESS]       = WGPUTextureFormat_BGRA8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BGRA8_UNORM_SRGB]     = WGPUTextureFormat_BGRA8UnormSrgb;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BGRX8_TYPELESS]       = WGPUTextureFormat_BGRA8Unorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BGRX8_UNORM_SRGB]     = WGPUTextureFormat_BGRA8UnormSrgb;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC6H_TYPELESS]        = WGPUTextureFormat_BC6HRGBUfloat;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC6H_UF16]            = WGPUTextureFormat_BC6HRGBUfloat;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC6H_SF16]            = WGPUTextureFormat_BC6HRGBFloat;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC7_TYPELESS]         = WGPUTextureFormat_BC7RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC7_UNORM]            = WGPUTextureFormat_BC7RGBAUnorm;
        m_FmtToWGPUFmtMap[TEX_FORMAT_BC7_UNORM_SRGB]       = WGPUTextureFormat_BC7RGBAUnormSrgb;
        // clang-format on
    }

    WGPUTextureFormat operator[](TEXTURE_FORMAT TexFmt) const
    {
        VERIFY_EXPR(TexFmt < _countof(m_FmtToWGPUFmtMap));
        return m_FmtToWGPUFmtMap[TexFmt];
    }

private:
    WGPUTextureFormat m_FmtToWGPUFmtMap[TEX_FORMAT_NUM_FORMATS] = {};
};


WGPUTextureFormat TexFormatToWGPUFormat(TEXTURE_FORMAT TexFmt)
{
    static const TexFormatToWebGPUFormatMapper FormatMap;
    return FormatMap[TexFmt];
}

TEXTURE_FORMAT WGPUFormatToTexFormat(WGPUTextureFormat WGPUFmt)
{
    return {};
}

} // namespace Diligent
