#pragma once
#include <GCE/Core/Maths/Vector3.h>

struct commsElement
{
	gce::Vector3f32 position;
	gce::Vector3f32 rotation;
	bool Alive = true;
};
