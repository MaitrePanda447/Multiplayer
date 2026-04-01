#ifndef RENDER_SPLIT_SCREEN_WINDOW_H_INCLUDED
#define RENDER_SPLIT_SCREEN_WINDOW_H_INCLUDED

#include "RenderTarget.h"
#include "Window.h"
#include "define.h"

namespace gce
{
	class DynamicBuffer;
	class D12PipelineObject;
	class Texture;
	class Geometry;
	class RenderCamera;

	enum SplitScreenDisposition : int8
	{
		HORIZONTAL_2_PLAYERS,
		VERTICAL_2_PLAYERS,
		HORIZONTAL_4_PLAYERS,
		VERTICAL_4_PLAYERS,
		SQUARE_4_PLAYERS,
		SQUARE_6_PLAYERS,
		SQUARE_8_PLAYERS,
	};

	class SplitScreenWindow : public Window
	{
	public:
		SplitScreenWindow() = default;
		SplitScreenWindow(WStringView title, uint32 width, uint32 height, SplitScreenDisposition splitScreenForm);
		SplitScreenWindow(SplitScreenWindow const& other) = delete;
		SplitScreenWindow(SplitScreenWindow&& other) noexcept = delete;

		auto operator=(SplitScreenWindow const& other) = delete;
		auto operator=(SplitScreenWindow&& other) = delete;

		~SplitScreenWindow() override;

		void SetCamera(RenderCamera& camera) const { m_pRenderTarget->SetCamera(camera); }

		void Create(WStringView title, uint32 width, uint32 height, SplitScreenDisposition splitScreenForm);
		float32 GetAspectRatio() const override;

		void Begin3D();
		void SetScreenToDraw(uint8 screenID);
		void Draw(RenderItem const& item);
		void EndDraw();

		RenderTarget* GetRenderTarget();

	private:
		RenderTarget* m_pRenderTarget  = nullptr;
		ID3D12GraphicsCommandList* m_frameCmdList = nullptr;

		uint8 m_screenCount = 2;
		SplitScreenDisposition m_form = SplitScreenDisposition::HORIZONTAL_2_PLAYERS;

		Vector<D3D12_VIEWPORT> m_viewportList = {};
		Vector<D3D12_RECT> m_scissorsList = {};

	private:
		void OnWindowResize() override;
		void ResizeViewports();
	};

}

#endif