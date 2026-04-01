#pragma once

#include "Geometry.h"

#include <Aliases.h>

#include <GCE/Core/define.h>
#include <GCE/Render/GeometryFactory.h>

namespace sr
{
    class Cube : public Geometry
    {
    public:
        Cube() : Geometry(gce::GeometryFactory::CreateCubeGeo()) {}
        ~Cube() override = default;
    };
    
    class Sphere : public Geometry
    {
    public:
        Sphere() : Geometry(gce::GeometryFactory::CreateSphereGeo(0.5f, 20, 20)) {}
        ~Sphere() override = default;
    };

    class HalfSphere : public Geometry
    {
    public:
        HalfSphere() : Geometry(gce::GeometryFactory::CreateHalfSphereGeo(0.5f, 20, 20)) {}
        ~HalfSphere() override = default;
    };
    
    class Capsule : public Geometry
    {
    public:
        Capsule() : Geometry(gce::GeometryFactory::CreateCapsuleGeo(0.5f, 1.0f, 20, 20)) {}
        ~Capsule() override = default;
    };

    class IcoSphere : public Geometry
    {
    public:
        IcoSphere() : Geometry(gce::GeometryFactory::CreateIcoSphereGeo(0.5f, 1)) {}
        ~IcoSphere() override = default;
    };
    
    class Plane : public Geometry
    {
    public:
        Plane() : Geometry(gce::GeometryFactory::CreatePlaneGeo(1, 1)) {}
        ~Plane() override = default;
    };
    
    class Donut : public Geometry
    {
    public:
        Donut() : Geometry(gce::GeometryFactory::CreateDonutGeo(0.5f, 0.25f, 20, 20)) {}
        ~Donut() override = default;
    };
    
    class Cylinder : public Geometry
    {
    public:
        Cylinder() : Geometry(gce::GeometryFactory::CreateCylinderGeo(0.5f, 0.5f, 1.0f, 20, 20)) {}
        ~Cylinder() override = default;
    };

    class Custom : public Geometry
    {
    public:
        Custom(String filepath) : Geometry(gce::GeometryFactory::LoadGeometry(RES_PATH + filepath)) {}
    };
}