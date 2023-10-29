#include "Fpch.h"
#include "Query.h"

void ecs::Query::VisitArchetypes( ECSManager& ecsManager, const VisitFunc& visitFunc ) const
{
	ecsManager.VisitAllArchetypes( [ & ]( ecs::ArchetypeView view )
		{
			if ( view.GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, m_includedFragments )
				&& !view.GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
			{
				visitFunc( view );
			}
		} );
}

void ecs::Query::VisitArchetypes( ECSManager& ecsManager, const VisitFuncWithCommands& visitFunc ) const
{
	DelayedCommands commands;

	ecsManager.VisitAllArchetypes( [ & ]( ecs::ArchetypeView view )
		{
			if ( view.GetArchetypeID().ContainsAllTagsAndFragments( m_includedTags, m_includedFragments )
				&& !view.GetArchetypeID().ContainsAnyTagsAndFragments( m_excludedTags, m_excludedFragments ) )
			{
				visitFunc( view, commands );
			}
		} );

	commands.Execute();
}
