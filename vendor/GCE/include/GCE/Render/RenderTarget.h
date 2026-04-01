#ifndef RENDER_RENDER_TARGET_H_INCLUDED
#define RENDER_RENDER_TARGET_H_INCLUDED

#include <d3d12.h>
#include <DirectXMath.h>
#include "define.h"
#include "DynamicBuffer.h"
#include "Texture.h"
#include "Structs.h"

namespace gce
{
	class DynamicBuffer;
	class Window;
	class D12ComputePipelineObject;
	class D12PipelineObject;
	class Geometry;
	class RenderCamera;
	class ShadowMap;
	class CubeMap;
	class ParticlesEmitter;

	class RenderTarget
	{
	public:

		RenderTarget(uint32 const width, uint32 const height, Window& window);
		RenderTarget(RenderTarget const& other) = delete;
		RenderTarget(RenderTarget&&) noexcept = delete;

		auto operator=(RenderTarget const& other) = delete;
		auto operator=(RenderTarget&& other) = delete;

		~RenderTarget();

		void SetCamera(RenderCamera& camera);
		void SetLights(std::vector<LightData>& lights);
		void ClearLights();
		LightData& GetLight(int index);
		int GetLightCount() const;

		static LightData CreateDirectionalLight(const Vector3f32& direction, const Vector4& color, float intensity, float strength);
		static LightData CreatePointLight(const Vector3f32& position, const Vector4& color, float range, float intensity, float strength);
		static LightData CreateSpotLight(const Vector3f32& position, const Vector3f32& direction, const Vector4& color, float spotAngle, float penumbraAngle, float range, float intensity, float strength);

		virtual void Begin3D();
		virtual void Draw(RenderItem const& item);
		virtual void DrawSkyBox(CubeMap const& skybox);
		virtual void DrawParticles(ParticlesEmitter& particleEmitter);
		virtual void End();

		void Resize(uint32 width, uint32 height);

		static ID3D12DescriptorHeap* GetDsvHeap() { return m_DSVHeap; }
		void ComputeParticlesUpdate(ParticlesEmitter& particleEmitter);

	protected:
		void CreateRTVHeap();
		void CreateDSVHeap();

		void RetrieveRTBuffer();
		void RetrieveDSBuffer();

		void CreateViewport();
		void UpdateViewport();
		//void UpdatePassData();

	protected:
		ID3D12Resource* m_renderTargetBuffer = nullptr;
		ID3D12Resource* m_depthStencilBuffer = nullptr;
		ID3D12DescriptorHeap* m_RTVHeap = nullptr;
		static ID3D12DescriptorHeap* m_DSVHeap;
		ID3D12GraphicsCommandList* m_pFrameCmdList = nullptr;

		D3D12_RECT m_scissorRect = {};
		D3D12_VIEWPORT m_viewport = {};

		uint32 m_width = 0;
		uint32 m_height = 0;

		int32 m_lightCount = 0;
		LightPassData m_lightPassData;

		Window* m_pWindow;

		friend class RenderWindow;
		friend class Window;
		friend class SplitScreenWindow;
	};

}

#endif