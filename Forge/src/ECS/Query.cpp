#include "Fpch.h"
#include "Query.h"

void ecs::Query::VisitArchetypes( ECSManager& ecsManager, const VisitFunc& visitFunc ) const
{
	auto archetypes = ecsManager.GetArchetypes();

	for( auto& archetype : archetypes )
	{
		if( archetype->GetArchetypeID().ContainsTagsAndFragments( m_tags, m_fragments ) )
		{
			visitFunc( *archetype );
		}
	}
}
