cmake_minimum_required (VERSION 3.10)

project(Diligent-GraphicsEngineWebGPU CXX)

set(INCLUDE
    include/BufferViewWebGPUImpl.hpp
    include/BufferWebGPUImpl.hpp
    include/DeviceContextWebGPUImpl.hpp
    include/EngineWebGPUImplTraits.hpp
    include/FenceWebGPUImpl.hpp
    include/FramebufferWebGPUImpl.hpp
    include/PipelineResourceAttribsWebGPU.hpp
    include/PipelineResourceSignatureWebGPUImpl.hpp
    include/PipelineStateWebGPUImpl.hpp
    include/QueryWebGPUImpl.hpp
    include/RenderDeviceWebGPUImpl.hpp
    include/RenderPassWebGPUImpl.hpp
    include/SamplerWebGPUImpl.hpp
    include/ShaderResourceBindingWebGPUImpl.hpp
    include/ShaderResourceCacheWebGPU.hpp
    include/ShaderVariableManagerWebGPU.hpp
    include/ShaderWebGPUImpl.hpp
    include/SwapChainWebGPUImpl.hpp
    include/TextureViewWebGPUImpl.hpp
    include/TextureWebGPUImpl.hpp
    include/WebGPUObjectWrappers.hpp
    include/WebGPUTypeConversions.hpp
)

set(INTERFACE 
   interface/DeviceContextWebGPU.h
   interface/EngineFactoryWebGPU.h
   interface/RenderDeviceWebGPU.h
   interface/SwapChainWebGPU.h
)

set(SRC 
   src/DeviceContextWebGPUImpl.cpp
   src/EngineFactoryWebGPU.cpp
   src/RenderDeviceWebGPUImpl.cpp
   src/SwapChainWebGPUImpl.cpp
   src/TextureWebGPUImpl.cpp
   src/WebGPUTypeConversions.cpp
)

set(DLL_SRC
    src/DLLMain.cpp
    src/GraphicsEngineWebGPU.def
)

add_library(Diligent-GraphicsEngineWebGPUInterface INTERFACE)
target_link_libraries     (Diligent-GraphicsEngineWebGPUInterface INTERFACE Diligent-GraphicsEngineInterface)
target_include_directories(Diligent-GraphicsEngineWebGPUInterface INTERFACE interface)

add_library(Diligent-GraphicsEngineWebGPU-static STATIC 
    ${SRC} ${INTERFACE} ${INCLUDE}
    readme.md
)

add_library(Diligent-GraphicsEngineWebGPU-shared SHARED 
    readme.md
)

if(MSVC)
    target_sources(Diligent-GraphicsEngineWebGPU-shared PRIVATE ${DLL_SRC})
endif()

set_dll_output_name(Diligent-GraphicsEngineWebGPU-shared GraphicsEngineWebGPU)

set_common_target_properties(Diligent-GraphicsEngineWebGPU-shared)
set_common_target_properties(Diligent-GraphicsEngineWebGPU-static)

target_include_directories(Diligent-GraphicsEngineWebGPU-static 
PRIVATE
    include
)

target_link_libraries(Diligent-GraphicsEngineWebGPU-static
PRIVATE
    Diligent-BuildSettings
    Diligent-TargetPlatform
    Diligent-Common
    Diligent-GraphicsEngine
    Diligent-GraphicsEngineNextGenBase
    Diligent-ShaderTools 
    webgpu
PUBLIC
    Diligent-GraphicsEngineWebGPUInterface
)

target_link_libraries(Diligent-GraphicsEngineWebGPU-shared
PRIVATE
    Diligent-BuildSettings 
    Diligent-GraphicsEngineWebGPU-static
PUBLIC
    Diligent-GraphicsEngineWebGPUInterface
)
target_compile_definitions(Diligent-GraphicsEngineWebGPU-shared PUBLIC ENGINE_DLL=1)
target_copy_webgpu_binaries(Diligent-GraphicsEngineWebGPU-shared)

source_group("src" FILES ${SRC})
source_group("dll" FILES ${DLL_SRC})
source_group("include" FILES ${INCLUDE})
source_group("interface" FILES ${INTERFACE})

set_target_properties(Diligent-GraphicsEngineWebGPU-static PROPERTIES
    FOLDER DiligentCore/Graphics
)
set_target_properties(Diligent-GraphicsEngineWebGPU-shared PROPERTIES
    FOLDER DiligentCore/Graphics
)

set_source_files_properties(
    readme.md PROPERTIES HEADER_FILE_ONLY TRUE
)

if(DILIGENT_INSTALL_CORE)
    install_core_lib(Diligent-GraphicsEngineWebGPU-shared)
    install_core_lib(Diligent-GraphicsEngineWebGPU-static)
endif()