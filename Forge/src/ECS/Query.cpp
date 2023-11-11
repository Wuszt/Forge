#include "Fpch.h"
#include "Query.h"

void ecs::Query::VisitArchetypes( const VisitFunc& visitFunc ) const
{
	const FragmentsFlags combinedFragmentFlags = m_includedFragments | m_includedMutableFragments;

	m_ecsManager.VisitAllMutableArchetypes( m_includedMutableFragments, m_includedFragments, [ & ]( ecs::MutableArchetypeView view )
		{
			if ( view.GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, combinedFragmentFlags )
				&& !view.GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
			{
				visitFunc( view );
			}
		} );
}

void ecs::Query::VisitArchetypes( const VisitFuncWithCommands& visitFunc ) const
{
	DelayedCommands commands;
	const FragmentsFlags combinedFragmentFlags = m_includedFragments | m_includedMutableFragments;

	m_ecsManager.VisitAllMutableArchetypes( m_includedMutableFragments, m_includedFragments, [ & ]( ecs::MutableArchetypeView view )
		{
			if ( view.GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, combinedFragmentFlags )
				&& !view.GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
			{
				visitFunc( view, commands );
			}
		} );

	commands.Execute();
}
