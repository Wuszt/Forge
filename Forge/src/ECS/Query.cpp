#include "Fpch.h"
#include "Query.h"

void ecs::Query::VisitArchetypes( const VisitFunc& visitFunc ) const
{
	m_ecsManager.VisitAllArchetypes( [ & ]( ecs::MutableArchetypeView view )
		{
			if ( view.GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, m_includedFragments )
				&& !view.GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
			{
				visitFunc( view );
			}
		} );
}

void ecs::Query::VisitArchetypes( const VisitFuncWithCommands& visitFunc ) const
{
	DelayedCommands commands;

	m_ecsManager.VisitAllArchetypes( [ & ]( ecs::MutableArchetypeView view )
		{
			if ( view.GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, m_includedFragments )
				&& !view.GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
			{
				visitFunc( view, commands );
			}
		} );

	commands.Execute();
}
