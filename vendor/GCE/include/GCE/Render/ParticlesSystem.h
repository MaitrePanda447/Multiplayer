#ifndef RENDER_PARTICLES_SYSTEM_H_INCLUDED
#define RENDER_PARTICLES_SYSTEM_H_INCLUDED

#define MAX_PARTICLES 20'000'000

// DX12
#include <d3d12.h>

#include "define.h"
#include "Structs.h"
#include "D12PipelineObject.h"
#include "D12ComputePipelineObject.h"
#include "StaticBuffer.h"

#include "../Core/Maths.h"

namespace gce
{

	struct ParticlesProperties
	{
		Vector3f32 position;
		int32 vertexCount;

		Vector3f32 velocity;
		int32 isAlive;

		Vector3f32 scale;
		int32 isCreated;

		Vector4 color;

		float64 duration;
		float64 respawnCooldown;
	};
	
	struct ParticleEmitterProperties
	{
		Vector3f32 position; // Emitter Pos
		Vector3f32 direction;// Particles Global Direction
		Vector3f32 directionOffset; // Particles Direction Offset that will randomly change for each particles
		Vector3f32 particleEmitterScale; // global Particles Scale

		Color particlesColor; // particles Color
		Vector2f32 particleVelocityRange;// min and max Velocity for each particles
		Vector2f32 durationRange;// min and max duration for each particles
		uint64 particlesCount;

		Vector2f32 respawnDurationRange;// min and max respawnCooldown for each particles
		int32 drawInLoop; // generate particles continuously
		Vector2f32 distanceWithEmitterAtSpawnRange;// min and max distance with Emitter at spawn
		int32 padding;

		int32 padding2;
	};

	class ParticlesEmitter
	{
	public:

		ParticlesEmitter(ParticleEmitterProperties prop);
		~ParticlesEmitter();

		void StartEmit();
		void UpdateBufferData();
		void StopEmit();

		void SetEmitterMatrix(Matrix const& worldMAtrix);

		void SetID(int64 id) {  m_id = id; }
		void SetNumFramesDirty(int32 num) {  m_numFramesDirty = num; }
		void SetToDestroy(bool toDestroy) {  m_toDestroy = toDestroy; }
		int64 GetID() { return m_id; }
		int32 GetNumFramesDirty() { return m_numFramesDirty; }
		bool GetDestroyState() { return m_toDestroy; }
		int64 const& GetMaxParticlesCount() const { return m_maxParticlesCount; }

	private:
		D12PipelineObject const& GetPso() const { return *m_pso; }
		D12ComputePipelineObject const& GetPsoCompute() const { return *m_psoComputeUpdate; }
		D12ComputePipelineObject const& GetPsoComputeStart() const { return *m_psoComputeInstantiate; }
		D3D12_VERTEX_BUFFER_VIEW const& GetVertexBufferView() const { return m_vertexBufferView; }
		D3D12_INDEX_BUFFER_VIEW  const& GetIndexBufferView()  const { return m_indexBufferView; }

		StaticBuffer const& GetVertexBufferIn() const { return m_vertexBufferIn; }
		StaticBuffer const& GetParticlesBufferIn() const { return m_particlesBufferIn; }
		StaticBuffer const& GetIndicesBuffer() const { return m_indices; }

		int64 const& GetIndicesCount() const { return m_maxParticlesCount * 3; }

		void InitBuffer();

	protected:

		D12PipelineObject* m_pso = nullptr;
		D12ComputePipelineObject* m_psoComputeUpdate = nullptr;
		D12ComputePipelineObject* m_psoComputeInstantiate = nullptr;

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};

		StaticBuffer m_vertexBufferIn;
		StaticBuffer m_particlesBufferIn;

		StaticBuffer m_indices;

		int64 m_maxParticlesCount = MAX_PARTICLES;

		EmitterData m_bufferData;

		ParticleEmitterProperties m_properties;

		uint32 m_id;
		int32 m_numFramesDirty;
		bool m_toDestroy = false;

		friend class RenderTarget;
	};
}

#endif