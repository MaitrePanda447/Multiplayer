#ifndef RENDER_RENDER_WINDOW_H_INCLUDED
#define RENDER_RENDER_WINDOW_H_INCLUDED

#include "DynamicBuffer.h"
#include "RenderTarget.h"
#include "Window.h"
#include "Structs.h"

namespace gce
{

	class D12PipelineObject;
	class Geometry;
	class RenderCamera;
	class DynamicBuffer;

	class RenderWindow : public Window // to see public Window + public RenderTarget
	{
	public:
		RenderWindow() = default;
		RenderWindow(WStringView title, int32 width, int32 height);
		RenderWindow(RenderWindow const& other) = delete;
		RenderWindow(RenderWindow&& other) noexcept = delete;

		auto operator=(RenderWindow const& other) = delete;
		auto operator=(RenderWindow&& other) = delete;

		~RenderWindow() override;

		void Create(WStringView title, int32 width, int32 height) override;
		void SetCamera(RenderCamera& camera) const { m_pTarget->SetCamera(camera); }


		void Begin3D();
		void Draw(RenderItem const& item);
		void EndDraw();

		RenderTarget* GetRenderTarget();

	private:
		RenderTarget* m_pTarget = nullptr;
		ID3D12GraphicsCommandList* m_frameCmdList = nullptr;

	private:
		void OnWindowResize() override;
	};
	
}

#endif