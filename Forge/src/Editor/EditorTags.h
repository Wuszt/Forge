#pragma once
#include "../ECS/Tag.h"

namespace editor
{
	struct EditorObjectTag : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( EditorObjectTag, ecs::Tag );
	};
}