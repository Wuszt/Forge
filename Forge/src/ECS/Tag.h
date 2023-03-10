#pragma once

namespace ecs
{
	struct Tag
	{
		DECLARE_STRUCT( Tag );

		static const Uint32 c_maxTagsAmount = 32u;

		using IndicesContainer = std::unordered_map< const Tag::Type*, Uint32 >;
		static Uint32 GetTagIndex( const Tag::Type& tagType )
		{
			IndicesContainer& indicesLUT = GetIndices();

			auto found = indicesLUT.find( &tagType );
			if ( found == indicesLUT.end() )
			{
				indicesLUT[ &tagType ] = static_cast< Uint32 >( indicesLUT.size() );
				FORGE_ASSERT( static_cast< Uint32 >( indicesLUT.size() ) < c_maxTagsAmount );
				return static_cast< Uint32 >( indicesLUT.size() ) - 1u;
			}

			return found->second;
		}

#ifdef FORGE_DEBUGGING
		static const Tag::Type* GetDebugTagTypeFromIndex( Uint32 index )
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

	using TagsFlags = std::bitset< Tag::c_maxTagsAmount >;
}