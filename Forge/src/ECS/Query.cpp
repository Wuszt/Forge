#include "Fpch.h"
#include "Query.h"

void ecs::Query::VisitArchetypes( ECSManager& ecsManager, const VisitFunc& visitFunc ) const
{
	auto archetypes = ecsManager.GetArchetypes();

	for( auto& archetype : archetypes )
	{
		if( archetype->GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, m_includedFragments ) && 
			!archetype->GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ))
		{
			visitFunc( *archetype );
		}
	}
}

void ecs::Query::VisitArchetypes( ECSManager& ecsManager, const VisitFuncWithCommands& visitFunc ) const
{
	DelayedCommands commands;
	auto archetypes = ecsManager.GetArchetypes();

	for ( auto& archetype : archetypes )
	{
		if ( archetype->GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, m_includedFragments ) &&
			!archetype->GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
		{
			visitFunc( *archetype, commands );
		}
	}

	commands.Execute();
}
