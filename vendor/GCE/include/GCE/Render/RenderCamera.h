#ifndef RENDER_RENDER_CAMERA_H_INCLUDED
#define RENDER_RENDER_CAMERA_H_INCLUDED

#include <DirectXMath.h>
#include "DynamicBuffer.h"
#include "Structs.h"

using namespace DirectX;


namespace gce
{
	enum CAMERA_TYPE : char
	{
		PERSPECTIVE, ORTHOGRAPHIC, TWO_DIMENSIONAL
	};

	class RenderCamera
	{
	public:

		RenderCamera() = default;
		RenderCamera(CAMERA_TYPE type) : m_type(type) {}
		RenderCamera(RenderCamera const& other) = delete;
		RenderCamera(RenderCamera&& other) noexcept = delete;

		RenderCamera& operator=(RenderCamera const& other) = delete;
		RenderCamera& operator=(RenderCamera&& other) = delete;

		~RenderCamera() = default;

		void UpdateVPMatrix();

		struct PerspectiveSettings
		{
			float32 fov;
			float32 aspectRatio;
			float32 nearPlane;
			float32 farPlane;
			Vector3f32 up;
		};

		struct OrthographicSettings
		{
			float32 viewWidth;
			float32 viewHeight;
			float32 nearPlane;
			float32 farPlane;
		};

		union
		{
			PerspectiveSettings perspective = {};
			union
			{
				OrthographicSettings orthographic;
				OrthographicSettings twoDimensional;
			};
		};

		void SetMatrix(Matrix const& matrix) { m_world = matrix; UpdateVPMatrix(); }

		Matrix& GetWMatrix() { return m_world; };
		Matrix& GetVPMatrix() { return m_viewProj; };

		Matrix m_proj = {};
	private:
		CAMERA_TYPE m_type = PERSPECTIVE;

		Matrix m_world = {};
		Vector3f32 m_forward{ 0.0f, 0.0f, 1.0f };
		Vector3f32 m_up = { 0.0f, 1.0f, 0.0f };

		Matrix m_viewProj = {};

		DynamicBuffer m_buffer{ sizeof(PassData) };

		friend class RenderWindow;
		friend class RenderTarget;
	};
}
#endif