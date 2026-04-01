#pragma once

#ifndef RENDER_CUBE_SHADOW_MAP_H_INCLUDED
#define RENDER_CUBE_SHADOW_MAP_H_INCLUDED

#include "RenderContext.h"
#include "Structs.h"
#include "../Core/Maths/Matrix.h"
#include "../Core/Maths/Vector3.h"

namespace gce
{
	class RenderCamera;
	struct RenderItem;
	class D12PipelineObject;

	class ShadowCubeMap
	{
		struct ShadowCubeMapDesc
		{
			uint32 size = 1024;
			DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DXGI_FORMAT resourceFormat = DXGI_FORMAT_R24G8_TYPELESS;
			DXGI_FORMAT srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			float32 biasConstant = 0.0001f;
			float32 biasSlope = 0.001f;
			float32 nearPlane = 0.1f;
			float32 farPlane = 20.0f;
		};

	public:
		ShadowCubeMap(const ShadowCubeMapDesc& desc);
		ShadowCubeMap(uint32 size, float32 nearPlane = 0.1f, float32 farPlane = 100.0f);

		ShadowCubeMap(const ShadowCubeMap&) = delete;
		ShadowCubeMap(ShadowCubeMap&&) = delete;
		ShadowCubeMap& operator =(const ShadowCubeMap&) = delete;
		ShadowCubeMap& operator =(ShadowCubeMap&&) = delete;
		~ShadowCubeMap() = default;

		UINT GetSize() const { return m_width; }
		float32 GetBiasConstant() const { return m_desc.biasConstant; }
		float32 GetBiasSlope() const { return m_desc.biasSlope; }
		float32 GetFarPlane() const { return m_desc.farPlane; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrv() const { return m_hCpuSrv; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrv() const { return m_hGpuSrv; }
		ID3D12Resource* GetResource() const { return m_pCubeShadowMap; }

		void BuildDescriptors(
			D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
			D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
			D3D12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

		void BeginShadowPass();
		void BeginCubeFace(uint32 faceIndex, const Vector3f32& lightPos);
		void DrawShadowMap(RenderItem& item, D12PipelineObject& pso);
		void EndCubeFace();
		void EndShadowPass();

		void SetLightPosition(const Vector3f32& lightPos);
		ShadowCubeData GetShadowCubeData() const;
		void SetBias(float32 constant, float32 slope);
		bool IsValid() const { return m_pCubeShadowMap != nullptr; }

	private:
		void CreateResource();
		void CreateDescriptors();
		void CalculateShadowMatrices();
		void SetupCubeFaceMatrices(const Vector3f32& lightPos);

		ShadowCubeMapDesc m_desc;
		UINT m_width;
		UINT m_height;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuRtv[6];
		D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuSrv;
		D3D12_GPU_DESCRIPTOR_HANDLE m_hGpuSrv;
		D3D12_CPU_DESCRIPTOR_HANDLE m_hCpuDsv[6];

		ID3D12Resource* m_pCubeShadowMap = nullptr;
		ID3D12GraphicsCommandList* m_pFrameCommandList = nullptr;

		ID3D12DescriptorHeap* m_pRtvHeap = nullptr;

		Matrix m_shadowMatrices[6];
		Matrix m_lightViewMatrices[6];
		Matrix m_lightProjection;
		Vector3f32 m_lightPosition;

		static const Vector3f32 s_cubeDirections[6];
		static const Vector3f32 s_cubeUpVectors[6];
	};
}

#endif
