#pragma once
#define REGISTER_TAG(...) static int GetTagIndex() { static int id = GetNextID(); return id; }

namespace ecs
{
	struct Tag
	{
		static const Uint32 c_maxTagsAmount = 128u;

	protected:
		static int GetNextID()
		{
			static int id = 0;
			return id++;
		}
	};
}