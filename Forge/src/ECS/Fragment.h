#pragma once
#define REGISTER_ECS_FRAGMENT(...) static int GetFragmentIndex() { static int id = GetNextID(); return id; }

namespace ecs
{
	struct Fragment
	{
		DECLARE_STRUCT( Fragment );

		static const Uint32 c_maxFragmentsAmount = 32u;

	protected:
		static int GetNextID()
		{
			static int id = 0;
			return id++;
		}
	};

	using FragmentsFlags = std::bitset< Fragment::c_maxFragmentsAmount >;
}