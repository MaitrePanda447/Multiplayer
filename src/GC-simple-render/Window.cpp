#include "Window.h"

#include <GCE/Render/RenderContext.h>

#include <chrono>

#include "Camera.h"
#include "Text.h"
#include "Geometries/Geometry.h"
#include "Inputs/InputSystem.h"
using namespace gce;

namespace sr
{
	Window::Window(std::wstring_view const title, uint32 const width, uint32 const height, int32 const flags)
	{
		Create(title, width, height, flags);
	}

	Window::~Window()
	{
		m_renderer.Destroy();
	}

	void Window::Create(std::wstring_view const title, uint32 const width, uint32 const height,int32 const flags)
	{
		RenderContext::Init();
		RenderContext::InitUI();

		m_IsShadowMapEnabled = (flags & FLAG_ENABLE_SHADOWPASS) != 0;
		
		if (flags & FLAG_NO_MSAA)
			RenderContext::SetMSAA(MSAA_COUNT::NONE);
		else if (flags & FLAG_MSAA_X4)
			RenderContext::SetMSAA(MSAA_COUNT::MSAA_COUNT_X4);
		else if (flags & FLAG_MSAA_X8)
			RenderContext::SetMSAA(MSAA_COUNT::MSAA_COUNT_X8);
		else
			RenderContext::SetMSAA(MSAA_COUNT::MSAA_COUNT_X2);

		m_window.Create(title, width, height);
		m_width = width;
		m_height = height;
		m_renderer.Init(this);

		m_window.OnWindowResizeCallBack([&](gce::Window* w)
		{
			m_renderer.ResizeCameraProj();	
		});
	}

	void Window::Begin(Camera& camera)
	{
		m_renderer.Begin3D(camera); //Begin frame
	}

	void Window::Draw(Geometry& geo)
	{
		m_geoToDraw.push_back(&geo); //Add to a list so we can iterate for our passes
	}

	void Window::DrawText(Text& text)
	{
		m_textToDraw.push_back(&text);
	}

	void Window::End()
	{
		//We must delay the draw inside the End3D function to encapsulate the multiples passes
		if (m_IsShadowMapEnabled)
			RenderShadowPass();
		RenderMainPass();
		
		m_renderer.End3D(); 
		Render2DPass();
		
		m_geoToDraw.clear();
		m_textToDraw.clear();
	}

	void Window::RenderShadowPass()
	{
		m_renderer.BeginShadow();
		
		for (Geometry* pGeo : m_geoToDraw)
			m_renderer.DrawShadowMap(pGeo->GetRenderItem());
		
		m_renderer.EndShadow();
	}

	void Window::RenderMainPass()
	{
		m_window.Begin3D();
		
		m_window.SetCamera(m_renderer.m_pCurrentCamera->GetGCRCamera());
		for (Geometry* pGeo : m_geoToDraw)
			m_window.Draw(pGeo->GetRenderItem());
		
		m_window.EndDraw();
	}

	void Window::Render2DPass()
	{
		m_window.BeginUI();

		//Get the client size to make the rect the whole screen. 
		RECT clientRect;
		GetClientRect(m_window.GetWindowHandle(), &clientRect);
		
		for (Text* pText : m_textToDraw)
		{
			m_window.SetActiveBrush(pText->m_brush);
			m_window.DrawTextUI(pText->m_text, *pText->m_font, RectanglePosF( pText->m_position.x, pText->m_position.y-15, clientRect.right, clientRect.bottom));
		}

		m_window.EndUI();
	}

	void Window::Display()
	{
		m_window.Display();
		InputSystem::HandleInputs();
		m_window.Update();
	}

	bool Window::IsOpen() const
	{
		// TO DO : Separate DeltaTime calculation into it's own function/abstraction
		static auto lastTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> delta = currentTime - lastTime;
		lastTime = currentTime;

		m_deltaTime = delta.count();

		return m_window.IsOpen();
	}

}
