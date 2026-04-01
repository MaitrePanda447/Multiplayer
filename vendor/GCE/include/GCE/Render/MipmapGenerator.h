#ifndef RENDER_MIPMAP_GENERATOR_H_INCLUDED
#define RENDER_MIPMAP_GENERATOR_H_INCLUDED

#include <d3d12.h>
#include "define.h"

namespace gce
{
	class StaticBuffer;
	class D12ComputePipelineObject;
    class D12PipelineObject;
    class Shader;
    class Texture;

    enum class TextureType : uint32;

    struct MipConstants
    {
        float32 texelSizeX;
        float32 texelSizeY;
        uint32  srcMipLevel;
        uint32 numMipLevel;
        uint32 textureType;
    };

    class MipmapGenerator
    {
    public:
        MipmapGenerator();
       ~MipmapGenerator();

       void GenerateMipmaps(ID3D12GraphicsCommandList* commandlist, StaticBuffer* buffer, TextureType textureType);
       void SetComputePSO(D12ComputePipelineObject* pso, D12PipelineObject* rootSig);
       bool IsInitialize() const;

    private:
        D12PipelineObject* m_rootSignature = nullptr;
        D12ComputePipelineObject* m_computePSO = nullptr;
        ID3D12DescriptorHeap* m_uavHeap = nullptr;

        void CreateTemporaryDescriptorHeap();

        void CreateMipViews(ID3D12Resource* resource,
            const D3D12_RESOURCE_DESC& desc,
            uint32 srcMip, uint32 dstMip,
            D3D12_CPU_DESCRIPTOR_HANDLE srvHandle,
            D3D12_CPU_DESCRIPTOR_HANDLE uavHandle);

        void FinalizeResourceState(ID3D12GraphicsCommandList* commandList,
            ID3D12Resource* resource, uint32 mipCount);

        void GenerateMipLevel(ID3D12GraphicsCommandList* commandList,
            ID3D12Resource* texture,
            uint32 srcMipLevel,
            uint32 mipCount,
            D3D12_GPU_DESCRIPTOR_HANDLE srcSRV,
            D3D12_GPU_DESCRIPTOR_HANDLE dstUAV, TextureType textureType);
    };
}
#endif