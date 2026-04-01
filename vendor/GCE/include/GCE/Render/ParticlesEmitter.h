#ifndef RENDER_PARTICLES_EMITTER_H_INCLUDED
#define RENDER_PARTICLES_EMITTER_H_INCLUDED

#define MAX_COLOR 10

// DX12
#include <d3d12.h>

#include "define.h"
#include "Structs.h"
#include "D12PipelineObject.h"
#include "D12ComputePipelineObject.h"
#include "StaticBuffer.h"
#include "DynamicBuffer.h"
#include "Maths.h"

namespace gce
{
	struct indicesAligned
	{
		int32 indice;
		int32 padding;
		int32 padding1;
		int32 padding2;
	};
	
	struct VertexPattern
	{
		Vector<Vertex> vertex;
		Vector<indicesAligned> indicesPattern;
		
		int32 vertexCount;
		int32 indicesCount;
		float64 paddding;
	};

	struct ParticlesPreset
	{
		static VertexPattern triangle;
		static VertexPattern square;

		//////////////////////////////////////
		/// @brief Loader for custom shapes
		/// @param path to obj from res
		//////////////////////////////////////
		static VertexPattern LoadPattern(String const& path);
	};

	struct ParticlesProperties
	{
		Matrix world;

		Vector3f32 velocity;
		int32 isAlive;

		Vector3f32 position;
		int32 isCreated;

		Vector3f32 scale;
		int32 padding;

		Vector4 quaternion;

		Vector4 color;

		float64 duration;
		float64 respawnCooldown = -5;
	};


	struct EmitterData
	{
		Vector3f32 direction; // Particles Global Direction
		int32 stopEmit;

		Vector2f32 scaleRange;
		int32 m_time;
		int32 particlesCount;

		Vector3f32 directionOffset; // Particles Direction Offset that will randomly change for each particles
		int32 isStarting; // Is ParticleSystem Starting

		Vector3f32 position; // Emitter Pos 
		int32 generateParticles; // generate particles continuously

		Vector2f32 particleVelocityRange; // min and max Velocity for each particles
		Vector2f32 durationRange; // min and max duration for each particles

		Vector2f32 respawnDurationRange; // min and max respawnCooldown for each particles
		Vector2f32 padding1;

		Vector3f32 positionOffset;
		int32 colorCount = 1;

		Vector4 particlesColor[MAX_COLOR]; // Color
	};
	
	struct ParticleEmitterProperties
	{
		Vector3f32 position; // Emitter Pos
		Vector3f32 direction;// Particles Global Direction
		Vector3f32 directionOffset; // Particles Direction Offset that will randomly change for each particles

		int32 colorCount = 1;
		Color particlesColor[MAX_COLOR]; // particles Color
		Vector2f32 particleVelocityRange;// min and max Velocity for each particles
		Vector2f32 durationRange;// min and max duration for each particles
		uint64 particlesCount;
		Vector2f32 respawnDurationRange;// min and max respawnCooldown for each particles
		int32 drawInLoop; // generate particles continuously
		Vector3f32 positionOffset;// min and max distance with Emitter at spawn
		Vector2f32 scaleRange; // Only use x and y for min and max
		VertexPattern vertexPattern = ParticlesPreset::triangle;
	};

	class ParticlesEmitter
	{
	public:

		/////////////////////////////////////////////////////////////////////////////
		/// @brief Create ParticlesEmitter and Instantiate buffers
		/// @param pPSO pso pointer for vs and ps shaders
		/// @param pPSOComputeUpdate pso pointer for cs "ParticleSystem.cs"
		/// @warning if not Instantiate with properties it will not emit particles
		/////////////////////////////////////////////////////////////////////////////
		ParticlesEmitter(D12PipelineObject* pPSO, D12ComputePipelineObject* pPSOComputeUpdate);

		/////////////////////////////////////////////////////////////
		/// @brief Create ParticlesEmitter and Instantiate buffers
		/// @param pPSO pso pointer for vs and ps shaders
		/// @param pPSOComputeUpdate pso pointer for cs "ParticleSystem.cs"
		/// @param prop Particles Emitter properties choosen by user
		/////////////////////////////////////////////////////////////
		ParticlesEmitter(D12PipelineObject* pPSO, D12ComputePipelineObject* pPSOComputeUpdate, ParticleEmitterProperties const& prop);
		~ParticlesEmitter() = default;

		/////////////////////////////////////////////////////////////
		/// @brief Instantiate buffers
		/// @param prop Particles Emitter properties choosen by user
		/////////////////////////////////////////////////////////////
		void Instantiate(ParticleEmitterProperties const& prop);
		
		///////////////////////////////////////////////////
		/// @brief Start to emit particles.
		/// @note ParticleEmitter need to be Instantiate
		///////////////////////////////////////////////////
		void StartEmit();

		///////////////////////////////////////
		/// @brief Update emitterBufferData.
		///////////////////////////////////////
		void UpdateBufferData();

		/////////////////////////////////////
		/// @brief Stop to emit particles.
		/////////////////////////////////////
		void StopEmit();

		/////////////////////////////////////////////
		/// @brief Change Emitter Transform data
		/// @param emitterPos emitter new Position
		/// @param emitterScale Emitter new scale
		/// @param rotation Emitter new rotation
		/////////////////////////////////////////////
		void SetEmitterTransform(Vector3f32 const& emitterPos, Vector3f32 const&  emitterScale, Quaternion const& rotation);

		////////////////////////////
		/// @brief ParticlesCount
		////////////////////////////
		int64 const& GetMaxParticlesCount() const { return m_maxParticlesCount; }

	private:

		//////////////////////////////////////////////
		/// @brief Create ParticlesEmitter Buffers.
		//////////////////////////////////////////////
		void InitBuffer();
		
		////////////////////////
		/// @brief Pso Getter
		////////////////////////
		D12PipelineObject const& GetPso() const { return *m_pso; }
		
		///////////////////////////////
		/// @brief PsoCompute Getter
		///////////////////////////////
		D12ComputePipelineObject const& GetPsoCompute() const { return *m_psoComputeUpdate; }
		
		/////////////////////////////////////
		/// @brief VertexBufferView Getter
		/////////////////////////////////////
		D3D12_VERTEX_BUFFER_VIEW const& GetVertexBufferView() const { return m_vertexBufferView; }
		
		////////////////////////////////////
		/// @brief IndexBufferView Getter
		////////////////////////////////////
		D3D12_INDEX_BUFFER_VIEW  const& GetIndexBufferView()  const { return m_indexBufferView; }
		
		/////////////////////////////////
		/// @brief VertexBuffer Getter
		/////////////////////////////////
		StaticBuffer const& GetVertexBufferIn() const { return m_vertexBufferIn; }

		////////////////////////////////////
		/// @brief ParticlesBuffer Getter
		////////////////////////////////////
		StaticBuffer const& GetParticlesBufferIn() const { return m_particlesBufferIn; }

		//StaticBuffer const& GetIndicesBuffer() const { return m_indices; }

		//////////////////////////////////////////////////
		/// @brief IndicesCount for one particle Getter
		//////////////////////////////////////////////////
		int64 const GetIndicesCount() const { return m_properties.vertexPattern.indicesCount; }

		////////////////////////
		/// @brief Emitter Cb
		////////////////////////
		DynamicBuffer const& GetEmitterBuffer() const { return m_emitterBuffer; }

		void SetID(int64 id) { m_id = id; }
		void SetNumFramesDirty(int32 num) { m_numFramesDirty = num; }
		void SetToDestroy(bool toDestroy) { m_toDestroy = toDestroy; }
		int64 GetID() { return m_id; }
		int32 GetNumFramesDirty() { return m_numFramesDirty; }
		bool GetDestroyState() { return m_toDestroy; }

	private:

		uint8 m_isInstantiate = 0;

		D12PipelineObject* m_pso = nullptr;
		D12ComputePipelineObject* m_psoComputeUpdate = nullptr;
		//D12ComputePipelineObject* m_psoComputeInstantiate = nullptr;

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};

		StaticBuffer m_vertexBufferIn;
		StaticBuffer m_particlesBufferIn;

		StaticBuffer m_indices;

		int64 m_maxParticlesCount;

		EmitterData m_emitterBufferData;
		DynamicBuffer m_emitterBuffer{ sizeof(EmitterData) };

		ParticleEmitterProperties m_properties;

		uint32 m_id;
		int32 m_numFramesDirty;
		bool m_toDestroy = false;

		friend class RenderTarget;
		friend class FrameResourcesManager;
	};

}

#endif