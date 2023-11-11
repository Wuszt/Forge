#pragma once
#include "Tag.h"

namespace ecs
{
	class ArchetypeView;

	class Query
	{
	public:
		Query( ECSManager& ecsManager )
			: m_ecsManager( ecsManager )
		{}

		enum class RequirementType
		{
			Excluded,
			Included
		};

		void AddTagRequirement( const ecs::Tag::Type& type, ecs::Query::RequirementType requirement )
		{
			if ( requirement == RequirementType::Included )
			{
				FORGE_ASSERT( !m_excludedTags.Test( type ) );
				m_includedTags.Set( type, true );
			}
			else
			{
				FORGE_ASSERT( !m_includedTags.Test( type ) );
				m_excludedTags.Set( type, true );
			}
		}

		template< class T >
		void AddTagRequirement( ecs::Query::RequirementType requirement )
		{
			AddTagRequirement( T::GetTypeStatic(), requirement );
		}

		void RemoveTagRequirement( const ecs::Tag::Type& type )
		{
			m_includedTags.Set( type, false );
			m_excludedTags.Set( type, false );
		}

		template< class T >
		void RemoveTagRequirement()
		{
			RemoveTagRequirement( T::GetTypeStatic() );
		}

		void AddFragmentRequirement( const ecs::Fragment::Type& type, ecs::Query::RequirementType requirement )
		{
			if ( requirement == RequirementType::Included )
			{
				FORGE_ASSERT( !m_excludedFragments.Test( type ) );
				m_includedFragments.Set( type, true );
			}
			else
			{
				FORGE_ASSERT( !m_includedFragments.Test( type ) );
				m_excludedFragments.Set( type, true );
			}
		}

		template< class T >
		void AddFragmentRequirement( ecs::Query::RequirementType requirement )
		{
			AddFragmentRequirement( T::GetTypeStatic(), requirement );
		}

		template< class T >
		void RemoveFragmentRequirement()
		{
			m_includedFragments.Set( T::GetTypeStatic(), false );
			m_excludedFragments.Set( T::GetTypeStatic(), false );
		}

		TagsFlags GetRequiredTags() const
		{
			return m_includedTags;
		}

		FragmentsFlags GetRequiredFragments() const
		{
			return m_includedFragments;
		}

		TagsFlags GetExcludedTags() const
		{
			return m_excludedTags;
		}

		FragmentsFlags GetExcludedFragments() const
		{
			return m_excludedFragments;
		}

		class DelayedCommands
		{
		public:
			using CommandFunc = std::function< void() >;

			void AddCommand( CommandFunc command )
			{
				m_commands.emplace_back( std::move( command ) );
			}

			void Execute()
			{
				for ( const CommandFunc& cmd : m_commands )
				{
					cmd();
				}
			}

		private:	
			std::vector< CommandFunc > m_commands;
		};

		using VisitFunc = std::function< void( ecs::ArchetypeView ) >;
		void VisitArchetypes( const VisitFunc& visitFunc ) const;
		
		using VisitFuncWithCommands = std::function< void( ecs::ArchetypeView, DelayedCommands& commands ) >;
		void VisitArchetypes( const VisitFuncWithCommands& visitFunc ) const;

	private:
		TagsFlags m_includedTags;
		TagsFlags m_excludedTags;

		FragmentsFlags m_includedFragments;
		FragmentsFlags m_excludedFragments;

		ECSManager& m_ecsManager;
	};
}

