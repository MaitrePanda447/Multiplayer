#pragma once

#include "DriverPriority.h"

#include <Aliases.h>

#include <string_view>
#include <GCE/Render/RenderWindow.h>
#include <GCE/Render/define.h>

#include "Renderer.h"
#undef DrawText

namespace sr
{
    class Text;
    
    class Window
    {
    public:
        enum Flag : int32
        {
            FLAG_NO_MSAA            = 0x00000001,
            FLAG_MSAA_X4            = 0x00000010,
            FLAG_MSAA_X8            = 0x00000100,
            FLAG_ENABLE_SHADOWPASS    = 0x00001000,
        };
        
        Window() = default;
        Window(std::wstring_view title, uint32 width, uint32 height, int32 flags = 0);
        Window(Window const& other) = delete;
        Window(Window&& other) = delete;

        Window& operator=(Window const& other) = delete;
        Window& operator=(Window&& other) = delete;

        ~Window(); 

        void Begin(Camera& camera);
        void Draw(Geometry& geo);
        void DrawText(Text& text);
        void End();

        void Display();
        
        bool IsOpen() const;

		float GetDeltaTime() const { return m_deltaTime; }

        float GetAspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }
        uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }

    private:
        void Create(std::wstring_view title, uint32 width, uint32 height, int32 type);
        void RenderShadowPass();
        void RenderMainPass();
        void Render2DPass();
        Renderer& GetRenderer() { return m_renderer; }
        
    private:
		mutable float m_deltaTime = 0.0f; // The entire deltaTime calculation is bad, and is just a temporary solution.
        bool m_IsShadowMapEnabled;
        
        gce::RenderWindow m_window;
        Renderer m_renderer;
        
        uint32 m_width = 0;
        uint32 m_height = 0;

        std::vector<Geometry*> m_geoToDraw;
        std::vector<Text*> m_textToDraw;

        friend class Renderer;
        friend class ShadowMap;
    };   
}
