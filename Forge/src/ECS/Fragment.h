#pragma once
#define REGISTER_ECS_FRAGMENT(...) static int GetFragmentIndex() { static Uint32 id = InitializeAndGetID(GetTypeStatic()); return id; }

namespace ecs
{
	struct Fragment
	{
		DECLARE_STRUCT( Fragment );

		static const Uint32 c_maxFragmentsAmount = 32u;

#ifdef FORGE_DEBUGGING
		static const Fragment::Type* GetDebugFragmentTypeFromIndex( Uint32 index )
		{
			return GetDebugFragmentsTypes()[ index ];
		}
#endif

	protected:
		static Uint32 InitializeAndGetID( const Fragment::Type& fragmentType )
		{
			static Uint32 id = 0;
			FORGE_ASSERT( id < c_maxFragmentsAmount );
			const Uint32 newId = id++;

#ifdef FORGE_DEBUGGING
			GetDebugFragmentsTypes().resize(Math::Max(newId + 1u, static_cast< Uint32 >( GetDebugFragmentsTypes().size() )));
			GetDebugFragmentsTypes()[ newId ] = &fragmentType;
#endif

			return newId;
		}

#ifdef FORGE_DEBUGGING
		static std::vector< const Fragment::Type* >& GetDebugFragmentsTypes()
		{
			static std::vector< const Fragment::Type* > s_fragmentsTypes;
			return s_fragmentsTypes;
		}		
#endif

	};

	using FragmentsFlags = std::bitset< Fragment::c_maxFragmentsAmount >;
}