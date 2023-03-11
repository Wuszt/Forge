#pragma once
#include "TypeFlags.h"

namespace ecs
{
	struct Tag
	{
		DECLARE_STRUCT( Tag );

		static constexpr Uint32 c_maxTagsAmount = 64u;

		using IndicesLUT = std::unordered_map< const Tag::Type*, Uint32 >;
		using IndicesContainer = std::vector< const Tag::Type* >;

		static Uint32 GetTypeIndex( const Tag::Type& tagType )
		{
			IndicesLUT& indicesLUT = GetIndicesLUT();
			IndicesContainer& indices = GetIndices();

			auto found = indicesLUT.find( &tagType );
			if ( found == indicesLUT.end() )
			{
				indicesLUT[ &tagType ] = static_cast< Uint32 >( indices.size() );
				indices.emplace_back( &tagType );

				FORGE_ASSERT( static_cast< Uint32 >( indicesLUT.size() ) < c_maxTagsAmount );
				return static_cast< Uint32 >( indicesLUT.size() ) - 1u;
			}

			return found->second;
		}

		static const Tag::Type* GetTypeWithIndex( Uint32 index )
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
			static IndicesLUT indicesLUT;
			return indicesLUT;
		}
	};

	using TagsFlags = ecs::TypeFlags< Tag, Tag::c_maxTagsAmount >;
}

namespace std
{
	template<>
	struct std::hash< ecs::TagsFlags > {
		std::size_t operator()( const ecs::TagsFlags& tags ) const noexcept
		{
			return Math::CalculateHash( tags.GetRawBits() );
		}
	};
}