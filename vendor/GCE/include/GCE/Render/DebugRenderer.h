#ifndef RENDER_DEBUGRENDERER_H_INCLUDED
#define RENDER_DEBUGRENDERER_H_INCLUDED

#include "Core/define.h"
#include "../Core/Maths.h"
#include "../Render/d3dx12.h"
#include "Structs.h"

#include <vector>

constexpr int16 MAX_LINES = 400;
#define RED { 255.0f, 0.0f, 0.0f }

namespace gce {

	class D12PipelineObject;
	class DynamicBuffer;
	class Geometry;

////////////////////////////////////////////////////////////////////
/// @brief DebugRenderer is a Singleton used to debug 3D render. 
////////////////////////////////////////////////////////////////////
class DebugRenderer
{
public:
	// Prevent the use of copy or move constructor
	DebugRenderer(DebugRenderer const& other) = delete;
	DebugRenderer(DebugRenderer&& other) = delete;

	// Prevent the use of copy or move operator
	DebugRenderer& operator=(DebugRenderer const& other) = delete;
	DebugRenderer& operator=(DebugRenderer&& other) = delete;

	DebugRenderer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);
	DebugRenderer() = default;
	~DebugRenderer();

	static DebugRenderer& Instance();
	
	// 3D
	static DebugBox* CreateDebugBox(Vector3f32 scale, Vector3f32 rot, Vector3f32 pos, Vector3f32 color = RED);

	// 2D 
	static DebugLine& AddDebugLine(DebugLine line, Vector3f32 color = RED);
	static DebugLine& AddDebugLine(Vector3f32 from, Vector3f32 to, Vector3f32 color = RED);
	static DebugLine& AddDebugLine(Vector3f32 from, Vector3f32 dir, float32 length, Vector3f32 color = RED);
	static DebugLine& AddDebugLine(Vertex from, Vertex to, Vector3f32 color = RED);

	static DebugArrow* CreateDebugArrow(Vector3f32 from, Vector3f32 dir, float32 length, Vector3f32 color = RED);
	static DebugCircle* CreateDebugCircle(Vector3f32 center, Vector3f32 normal, float32 radius, Vector3f32 color = RED);

	void DrawDebugLines(gce::Matrix vp); 

private:
	void Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);
	bool PushLine(DebugLine& line);
	
	ID3D12Device* m_pDevice;
	ID3D12GraphicsCommandList* m_pCmdList;
	D12PipelineObject* m_pPSO;

	uint16 m_lineCount = 0;
	uint16 m_vertexCount = 0;

	std::array<DebugLine, MAX_LINES> m_lineArray;
	std::array<Vertex, MAX_LINES> m_vertexArray;
	D3D12_VERTEX_BUFFER_VIEW m_vertices;

	friend class RenderContext;
};

}
#endif
