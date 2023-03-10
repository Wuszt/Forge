#pragma once
#include "TypeFlags.h"

namespace ecs
{
	struct Fragment
	{
		DECLARE_STRUCT( Fragment );

		static const Uint32 c_maxFragmentsAmount = 32u;

		using IndicesContainer = std::unordered_map< const Fragment::Type*, Uint32 >;
		static Uint32 GetTypeIndex( const Fragment::Type& fragmentType )
		{
			IndicesContainer& indicesLUT = GetIndices();

			auto found = indicesLUT.find( &fragmentType );
			if ( found == indicesLUT.end() )
			{
				indicesLUT[ &fragmentType ] = static_cast< Uint32 >( indicesLUT.size() );
				FORGE_ASSERT( static_cast< Uint32 >( indicesLUT.size() ) < c_maxFragmentsAmount );
				return static_cast< Uint32 >( indicesLUT.size() ) - 1u;
			}

			return found->second;
		}

#ifdef FORGE_DEBUGGING
		static const Fragment::Type* GetDebugFragmentTypeFromIndex( Uint32 index )
		{
			IndicesContainer& indicesLUT = GetIndices();
			auto found = std::find_if( indicesLUT.begin(), indicesLUT.end(), [ index ]( const auto& entry )
				{
					return entry.second == index;
				} );

			if ( found == indicesLUT.end() )
			{
				return nullptr;
			}

			return found->first;
		}
#endif

	private:
		static IndicesContainer& GetIndices()
		{
			static IndicesContainer indices;
			return indices;
		}
	};

	using FragmentsFlags = ecs::TypeFlags< Fragment, Fragment::c_maxFragmentsAmount >;
};

namespace std
{
	template<>
	struct std::hash< ecs::FragmentsFlags > {
		std::size_t operator()( const ecs::FragmentsFlags& fragments ) const noexcept
		{
			return Math::CalculateHash( fragments.GetRawBits() );
		}
	};
}