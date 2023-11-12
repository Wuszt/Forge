#include "Fpch.h"
#include "Query.h"

void ecs::Query::VisitArchetypes( const VisitFunc& visitFunc ) const
{
	const FragmentsFlags combinedFragmentFlags = m_includedFragments | m_includedMutableFragments;

	for ( const auto& archetype : m_ecsManager.m_archetypes )
	{
		if ( archetype->GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, combinedFragmentFlags )
			&& !archetype->GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
		{
			m_ecsManager.TriggerOnBeforeReadingArchetype( *archetype, combinedFragmentFlags );
			m_ecsManager.TriggerOnBeforeModifyingArchetype( *archetype, m_includedMutableFragments );
		}
	}

	for ( const auto& archetype : m_ecsManager.m_archetypes )
	{
		if ( archetype->GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, combinedFragmentFlags )
			&& !archetype->GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
		{
			visitFunc( { *archetype, m_includedMutableFragments, m_includedFragments } );
		}
	}
}

void ecs::Query::VisitArchetypes( const VisitFuncWithCommands& visitFunc ) const
{
	DelayedCommands commands;
	const FragmentsFlags combinedFragmentFlags = m_includedFragments | m_includedMutableFragments;

	for ( const auto& archetype : m_ecsManager.m_archetypes )
	{
		if ( archetype->GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, combinedFragmentFlags )
			&& !archetype->GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
		{
			m_ecsManager.TriggerOnBeforeReadingArchetype( *archetype, combinedFragmentFlags );
			m_ecsManager.TriggerOnBeforeModifyingArchetype( *archetype, m_includedMutableFragments );
		}
	}

	for ( const auto& archetype : m_ecsManager.m_archetypes )
	{
		if ( archetype->GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, combinedFragmentFlags )
			&& !archetype->GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
		{
			visitFunc( { *archetype, m_includedMutableFragments, m_includedFragments }, commands );
		}
	}

	commands.Execute();
}
