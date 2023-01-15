#pragma once
#define REGISTER_ECS_TAG(...) static int GetTagIndex() { static int id = GetNextID(); return id; }

namespace ecs
{
	struct Tag
	{
		static const Uint32 c_maxTagsAmount = 32u;

	protected:
		static int GetNextID()
		{
			static int id = 0;
			FORGE_ASSERT( id < c_maxTagsAmount );
			return id++;
		}
	};

	using TagsFlags = std::bitset< Tag::c_maxTagsAmount >;
}