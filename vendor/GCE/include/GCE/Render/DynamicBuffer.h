#ifndef RENDER_DYNAMIC_BUFFER_H_INCLUDED
#define RENDER_DYNAMIC_BUFFER_H_INCLUDED

#include <d3d12.h>
#include <string_view>

#include "define.h"

namespace gce
{
	
	class DynamicBuffer
	{
	public:
		DynamicBuffer() = default;
		DynamicBuffer(uint32 size);
		DynamicBuffer(DynamicBuffer const& other) = delete;
		DynamicBuffer(DynamicBuffer&& other) = delete;

		DynamicBuffer& operator=(DynamicBuffer const& other) = delete;
		DynamicBuffer& operator=(DynamicBuffer&& other) noexcept = delete;

		~DynamicBuffer();

		void Init(uint32 size);
		template <typename T>
		void CopyData(T& data);
		template <typename T>
		void CopyDataPtr(T* pData, uint64 sizeofData);
		void SetName(std::wstring_view name) const { m_pBuffer->SetName(name.data()); }

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return m_pBuffer->GetGPUVirtualAddress(); }

	private:
		ID3D12Resource* m_pBuffer;
		void* m_pCPUPointer;

		friend class ParticlesEmitter;
	};

	template <typename T>
	void DynamicBuffer::CopyData(T& data)
	{
		memcpy(m_pCPUPointer, &data, sizeof(data));
	}

	template <typename T>
	void DynamicBuffer::CopyDataPtr(T* pData, uint64 sizeofData)
	{
		memcpy(m_pCPUPointer, pData, sizeofData);
	}
}

#endif