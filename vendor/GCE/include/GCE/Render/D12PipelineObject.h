#ifndef RENDER_PIPELINE_OBJECT_H_INLCUDED
#define RENDER_PIPELINE_OBJECT_H_INLCUDED

#include <d3d12.h>

#include "../Core/define.h"

namespace gce
{
	class Shader;

	enum DescriptorType : char8
	{
		DEFAULT,
		SKYBOX,
		SHADOW_MAP,
		WIREFRAME,
		CUBE_SHADOW_MAP,
		LINE, // DebugRenderer 
	};

	struct PSODescriptor
	{
		// Common descriptors
		DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		int32 numRenderTargets = 1;
		bool blendEnable = true;
		D3D12_BLEND srcBlend = D3D12_BLEND_SRC_ALPHA;
		D3D12_BLEND destBlend = D3D12_BLEND_INV_SRC_ALPHA;
		D3D12_BLEND_OP blendOp = D3D12_BLEND_OP_ADD;
		D3D12_BLEND srcBlendAlpha = D3D12_BLEND_ONE;
		D3D12_BLEND destBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		D3D12_BLEND_OP blendOpAlpha = D3D12_BLEND_OP_ADD;
		D3D12_LOGIC_OP logicOp = D3D12_LOGIC_OP_NOOP;
		float32 renderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		DescriptorType type = DEFAULT;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Shadow specific descriptors
		struct ShadowDesc
		{
			uint32 numRenderTargets = 0;
			DXGI_FORMAT RTVFormats = DXGI_FORMAT_UNKNOWN;
		} shadow;

		// SkyBox specific descriptors
		struct SkyBoxDesc
		{
			D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_NONE;
			D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		} sky;

		// Wireframe specific descriptors
		struct WireframeDesc
		{
			D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_WIREFRAME;
		} wireframe;

		struct LineDesc
		{
			D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		} line;
	};

	class D12PipelineObject
	{
	public:
		D12PipelineObject() = default;
		D12PipelineObject(Shader const& VS, Shader const& PS, Shader const& HS, Shader const& DS, Shader const& rootsig, PSODescriptor speDesc);
		D12PipelineObject(D12PipelineObject const& other) = delete;
		D12PipelineObject(D12PipelineObject&& other) noexcept = delete;
		explicit D12PipelineObject(Shader const& rootsig);

		D12PipelineObject& operator=(D12PipelineObject const& other) = delete;
		D12PipelineObject& operator=(D12PipelineObject&& other) = delete;

		ID3D12RootSignature* GetRoootSignature();

		~D12PipelineObject();
		D12PipelineObject(Shader const& VS, Shader const& PS, Shader const& rootsig, PSODescriptor speDesc);

	protected:
		void Init(Shader const& VS, Shader const& PS, Shader const& rootsig, PSODescriptor speDesc);

	private:
		ID3D12PipelineState* m_pPipelineState = nullptr;
		ID3D12RootSignature* m_pRootSignature = nullptr;

		D3D12_INPUT_ELEMENT_DESC m_inputLayout[5]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		friend class CubeMap;
		friend class RenderWindow;
		friend class RenderTarget;
		friend class ShadowMap;
		friend class ShadowCubeMap;
		friend class D12ComputePipelineObject;
		friend class DebugRenderer;
	};
}
#endif