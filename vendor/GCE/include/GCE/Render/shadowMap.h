#ifndef RENDER_SHADOWMAP_H_INCLUDED
#define RENDER_SHADOWMAP_H_INCLUDED

#include "RenderContext.h"
#include "RenderTarget.h"
#include "../Core/Maths/Matrix.h"

namespace gce
{
	class RenderCamera;
	struct RenderItem;

	struct ShadowMapDesc
	{
		uint32 size = 2048;
		DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT resourceFormat = DXGI_FORMAT_R24G8_TYPELESS;
		DXGI_FORMAT srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		float32 biasConstant = 0.0001f;
		float32 biasSlope = 0.001f;
	};

	class ShadowMap
	{
	public:

		explicit ShadowMap(const ShadowMapDesc& desc);
		ShadowMap(uint32 size);

		ShadowMap(const ShadowMap&) = delete;
		ShadowMap(ShadowMap&&) = delete;
		ShadowMap& operator=(const ShadowMap&) = delete;
		ShadowMap& operator=(ShadowMap&&) = delete;
		~ShadowMap() = default;


		UINT GetSize() const { return m_width; }
		float32 GetBiasConstant() const { return m_desc.biasConstant; }
		float32 GetBiasSlope() const { return m_desc.biasSlope; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrv() const { return m_hCpuSrv; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDsv() const { return m_hCpuDsv; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrv() const { return m_hGpuSrv; }
		ID3D12Resource* GetResource() const { return m_pShadowMap; }

		void BuildDescriptors(
			D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
			D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
			D3D12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

		void BeginShadow();
		void SetLightCamera( RenderCamera& cam, const Vector3f32& lightPos, const Vector3f32& lightDir);
		void DrawShadowMap(RenderItem& item, D12PipelineObject& pso); 
		void EndShadow();

		Matrix GetShadowMatrix() const { return m_shadowMatrix; }
		ShadowData GetShadowData() const;
		void SetBias(float32 constant, float32 slope);
		bool IsValid() const { return m_pShadowMap != nullptr; }
		
	private:
		void CreateResource();
		void CreateDescriptors();
		void CalculateShadowMatrix();

		ShadowMapDesc m_desc;
        UINT m_width;
        UINT m_height;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		D3D12_SHADER_RESOURCE_VIEW_DESC m_SRVDesc;
		D3D12_DEPTH_STENCIL_DESC m_DSVDesc;

		D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuSrv;
		D3D12_GPU_DESCRIPTOR_HANDLE m_hGpuSrv;
		D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuDsv;
		
		ID3D12Resource* m_pShadowMap = nullptr;
		ID3D12GraphicsCommandList* m_pFrameCommandList = nullptr;

		Matrix m_shadowMatrix;
		Matrix m_lightViewProj;

		friend class RenderTarget;
	};
}
#endif