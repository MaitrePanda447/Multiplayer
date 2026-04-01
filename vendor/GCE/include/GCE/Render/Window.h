#ifndef RENDER_WINDOW_H_INCLUDED
#define RENDER_WINDOW_H_INCLUDED

#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <Windows.h>
#include <vector>
#include <functional>

#include <d3d11on12.h>
#include <d2d1_1.h>
#include <wrl.h>

#include "define.h"
#include "Structs.h"
#include "../Core/Maths/Color.h"

namespace gce
{
	//class Camera;
	class FrameResource;
	struct RectanglePosF;
	class Font;

	class Window
	{
	public:
		Window() = default;
		Window(WStringView title, int32 width, int32 height);
		Window(Window const& other) = delete;
		Window(Window&& other) noexcept = delete;

		auto operator=(Window const& other) = delete;
		auto operator=(Window&& other) = delete;

		virtual ~Window();

		virtual void Create(WStringView title, int32 width, int32 height);
		void Update() const;
		void Display();

		virtual float32 GetAspectRatio() const { return min(10, static_cast<float32>(m_width) / static_cast<float32>(m_height)); }
		bool IsOpen() const { return m_isOpen; }
		uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		HWND GetWindowHandle() const { return m_windowHandle; }
		// float32 GetAspectRatio() const { return static_cast<float32>(m_width) / static_cast<float32>(m_height); }

		void OnWindowResizeCallBack(std::function<void(Window*)> const& func) { mResizeCallBack = func; }
		void SetTitle(std::wstring_view const& title) const;

		//== UI MODULE==

		void BeginUI();
		void SetActiveBrush(UiBrush& brush);
	
		void DrawShapeUI(ShapeUi& rectanglePos, bool fillRect);
		void DrawTextUI(std::wstring_view text, Font const&, RectanglePosF const& rectanglePos);
		
		void EndUI();

		static constexpr DXGI_FORMAT BackBufferFormat{ DXGI_FORMAT_B8G8R8A8_UNORM };
		static constexpr DXGI_FORMAT DepthStencilFormat{ DXGI_FORMAT_D24_UNORM_S8_UINT };
		static constexpr int32 s_swapChainBufferCount { 2 };

		inline static const DirectX::XMVECTORF32 sClearColor{ DirectX::Colors::LightBlue };

	protected:

		int32 m_currentBackBuffer = 0;
		IDXGISwapChain* m_swapChain = nullptr;

		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[s_swapChainBufferCount] = {};
		ID3D12Resource* m_swapChainBuffers[s_swapChainBufferCount] = {};
		ID3D12DescriptorHeap* m_swapChainBuffersHeap = nullptr;

		D3D12_VIEWPORT m_viewPort = {};
		D3D12_RECT m_scissorRect = {};

		ATOM m_windowClass = {};
		HINSTANCE m_HInstance = {};
		HWND m_windowHandle = {};

		bool m_isOpen = false;
		uint32 m_width = 0;
		uint32 m_height = 0;

		std::function<void(Window*)> mResizeCallBack;

		// == UI ==
		ID3D11Resource* m_WrappedTarget[s_swapChainBufferCount];
		ID2D1Bitmap1* m_2dRenderTarget[s_swapChainBufferCount];


		//TODO Move those frame resources
		ID2D1Brush* m_activeBrush = nullptr;

		//============

	protected:
		ID3D12Resource* GetCurrentBackBuffer() const { return m_swapChainBuffers[m_currentBackBuffer]; }
		virtual void OnWindowResize() {};

		void SetBitMapBrushProperties(String const& path);

	private:
		static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		LRESULT HandleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void ResizeViewport();
		void ResizeWindow(int32 newWidth, int32 newHeight);

		bool CreateWindowClass(std::wstring_view title, int32 width, int32 height);
		bool CreateSwapChain();

		bool CreateSwapChainBuffersHeap();

		bool RetrieveSwapChainBuffers();
		bool ReleaseSwapChainBuffers();

		bool Release2DTarget();
		bool Retrieve2DTargets();

		friend class RenderTarget;
		friend class MoveCamera;
		friend class FrameResource;
	};
	
}
#endif
