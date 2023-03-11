#pragma once
#include "TypeFlags.h"

namespace ecs
{
	struct Fragment
	{
		DECLARE_STRUCT( Fragment );

		static const Uint32 c_maxFragmentsAmount = 64u;

		using IndicesLUT = std::unordered_map< const Fragment::Type*, Uint32 >;
		using IndicesContainer = std::vector< const Fragment::Type* >;

		static Uint32 GetTypeIndex( const Fragment::Type& fragmentType )
		{
			IndicesLUT& indicesLUT = GetIndicesLUT();
			IndicesContainer& indices = GetIndices();

			auto found = indicesLUT.find( &fragmentType );
			if ( found == indicesLUT.end() )
			{
				indicesLUT[ &fragmentType ] = static_cast< Uint32 >( indices.size() );
				indices.emplace_back( &fragmentType );

				FORGE_ASSERT( static_cast< Uint32 >( indicesLUT.size() ) < c_maxFragmentsAmount );
				return static_cast< Uint32 >( indicesLUT.size() ) - 1u;
			}

			return found->second;
		}

		static const Fragment::Type* GetTypeWithIndex( Uint32 index )
		{
			IndicesContainer& indices = GetIndices();

			if ( index >= indices.size() )
			{
				return nullptr;
			}

			return indices[ index ];
		}

	private:
		static IndicesContainer& GetIndices()
		{
			static IndicesContainer indices;
			return indices;
		}

		static IndicesLUT& GetIndicesLUT()
		{
			static IndicesLUT indices;
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