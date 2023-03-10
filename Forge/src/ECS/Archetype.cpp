#include "Fpch.h"
#include "Archetype.h"

ecs::Archetype ecs::Archetype::GetEmptyCopy() const
{
	Archetype archetype( static_cast< Uint32 >( m_sparseSet.size() ) );
	for( const auto& fragmentPackage : m_fragments )
	{
		archetype.m_fragments.emplace( fragmentPackage.first, ecs::FragmentsPackage( *fragmentPackage.first ) );
	}

	archetype.m_id = GetArchetypeID();

	return archetype;
}
