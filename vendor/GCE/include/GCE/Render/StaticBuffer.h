#ifndef RENDER_STATIC_BUFFER_H_INCLUDED
#define RENDER_STATIC_BUFFER_H_INCLUDED

#include <d3d12.h>
#include "d3dx12.h"
#include "define.h"

namespace gce
{
	class Texture;
	class ImageLoader;
	struct ImageData;

	class StaticBuffer
	{
	public:
		StaticBuffer() = default;
		StaticBuffer(StaticBuffer const& other) = delete;
		StaticBuffer(StaticBuffer&& other) noexcept = delete;

		StaticBuffer& operator=(StaticBuffer const& other) = delete;
		StaticBuffer& operator=(StaticBuffer&& other) = delete;

		~StaticBuffer();

		void Init(void const* initData, uint64 initDataSize);
		void InitTexture(const ImageData& imgData, bool is3D = false);
		void InitTextureMipmap(const ImageData& imgData);
		void SetName(std::wstring_view name) const { m_defaultBuffer->SetName(name.data()); }

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return m_defaultBuffer->GetGPUVirtualAddress(); }

		ID3D12Resource* GetDefaultBuffer();

		uint32 GetMipLevels();

	private:
		ID3D12Resource* m_defaultBuffer = nullptr;
		ID3D12Resource* m_uploadBuffer = nullptr;
		uint32 m_mipLevels = 1;

		uint64 m_dataSize = 0;

		friend class RenderContext;
		friend class Texture;
		friend class Texture3D;
		friend class CubeMap;
		friend class ImageLoader;
		friend class ParticlesEmitter;
	};
}

#endif