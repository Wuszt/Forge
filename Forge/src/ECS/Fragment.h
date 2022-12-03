#pragma once
#define REGISTER_FRAGMENT(...) static int GetFragmentIndex() { static int id = GetNextID(); return id; }

namespace ecs
{
	struct Fragment
	{
		DECLARE_STRUCT( Fragment );

		static const Uint32 c_maxFragmentsAmount = 128u;

	protected:
		static int GetNextID()
		{
			static int id = 0;
			return id++;
		}
	};
}