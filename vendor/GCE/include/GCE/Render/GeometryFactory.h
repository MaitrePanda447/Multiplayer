#ifndef RENDER_GEOMETRY_FACTORY_H_INCLUDED
#define RENDER_GEOMETRY_FACTORY_H_INCLUDED

#include <vector>
#include "Geometry.h"

namespace gce
{
	class Geometry;
	struct Vertex;

	class GeometryFactory
	{
	public:
		static Geometry* CreateCubeGeo();
		static Geometry* CreateCapsuleGeo(float32 radius, float32 height, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateSphereGeo(float32 radius, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateHalfSphereGeo(float32 radius, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateIcoSphereGeo(float32 radius, uint32 numSubdivisions);
		static Geometry* CreateCylinderGeo(float32 bottomRadius, float32 topRadius, float32 height, uint32 sliceCount, uint32 stackCount);
		static Geometry* CreateDonutGeo(float32 majorRadius, float32 minorRadius, uint32 majorSegments, uint32 minorSegments);
		static Geometry* CreatePlaneGeo(float32 width, float32 height);
		static Geometry* Create2DPlane();
		static Geometry* CreatePlaneGeo3D(float32 width, float32 height, float32 thickness);

		static Geometry* LoadGeometry(std::string_view path);
	private:
		static Vertex MidPoint(Vertex const& v0, Vertex const& v1);
		static void Subdivide(std::vector<uint32>& indices, std::vector<Vertex>& vertices);
		static void BuildCylinderBody(float32 topRadius, float32 bottomRadius, float32 height, uint32 sliceCount, uint32 stackCount, std::vector<Vertex>& vertices, std::vector<uint32>& indices);
		static void BuildCylinderCap(float32 radius, float32 height, uint32 sliceCount, std::vector<Vertex>& vertices, std::vector<uint32>& indices, bool isTop);
		static void BuildHalfSphere(float32 topRadius, uint32 sliceCount, uint32 stackCount, std::vector<Vertex>& vertices, std::vector<uint32>& indices, bool isTop, float32 height = 0.0f);
	};
}
#endif