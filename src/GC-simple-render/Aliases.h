#ifndef ALIASES_H
#define ALIASES_H

#include <GCE/Core/Maths/Vector4.h>
#include <GCE/Core/Maths/Vector3.h>
#include <GCE/Core/Maths/Vector2.hpp>

#include <GCE/Core/Maths/Color.h>

#include <GCE/Render/Font.h>

namespace sr {
	// Maths Aliases
	using Vector4 = gce::Vector4;
	using Vector3 = gce::Vector3f32;
	using Vector2 = gce::Vector2f32;

	using Color = gce::Color;

	// Resources Aliases
	using Font = gce::Font;
}

#endif