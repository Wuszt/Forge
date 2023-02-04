#pragma once
#define REGISTER_ECS_TAG(...) static int GetTagIndex() { static int id = InitializeAndGetID( GetTypeStatic() ); return id; }

namespace ecs
{
	struct Tag
	{
		DECLARE_STRUCT( Tag );

		static const Uint32 c_maxTagsAmount = 32u;

#ifdef FORGE_DEBUGGING
		static const Tag::Type* GetDebugTagTypeFromIndex( Uint32 index )
		{
			return GetDebugTagTypes()[ index ];
		}
#endif

	protected:
		static Uint32 InitializeAndGetID( const Tag::Type& tagType )
		{
			static Uint32 id = 0;
			FORGE_ASSERT( id < c_maxTagsAmount );
			const Uint32 newId = id++;

#ifdef FORGE_DEBUGGING
			GetDebugTagTypes().resize( Math::Max( newId + 1u, static_cast< Uint32 >( GetDebugTagTypes().size() ) ) );
			GetDebugTagTypes()[ newId ] = &tagType;
#endif

			return newId;
		}

#ifdef FORGE_DEBUGGING
		static std::vector< const Tag::Type* >& GetDebugTagTypes()
		{
			static std::vector< const Tag::Type* > s_tagTypes;
			return s_tagTypes;
		}
#endif
	};

	using TagsFlags = std::bitset< Tag::c_maxTagsAmount >;
}