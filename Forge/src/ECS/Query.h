#pragma once
#include "Tag.h"

namespace ecs
{
	class Archetype;

	class Query
	{
	public:

		enum class RequirementType
		{
			Excluded,
			Included
		};

		void AddTagRequirement( const ecs::Tag::Type& type, ecs::Query::RequirementType requirement )
		{
			if ( requirement == RequirementType::Included )
			{
				FORGE_ASSERT( !m_excludedTags.test( Tag::GetTagIndex( type ) ) );
				m_includedTags.set( Tag::GetTagIndex( type ) );
			}
			else
			{
				FORGE_ASSERT( !m_includedTags.test( Tag::GetTagIndex( type ) ) );
				m_excludedTags.set( Tag::GetTagIndex( type ) );
			}
		}

		template< class T >
		void AddTagRequirement( ecs::Query::RequirementType requirement )
		{
			AddTagRequirement( T::GetTypeStatic(), requirement );
		}

		void RemoveTagRequirement( const ecs::Tag::Type& type )
		{
			m_includedTags.reset( Tag::GetTagIndex( type ) );
			m_excludedTags.reset( Tag::GetTagIndex( type ) );
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
				FORGE_ASSERT( !m_excludedFragments.test( Fragment::GetFragmentIndex( type ) ) );
				m_includedFragments.set( Fragment::GetFragmentIndex( type ) );
			}
			else
			{
				FORGE_ASSERT( !m_includedFragments.test( Fragment::GetFragmentIndex( type ) ) );
				m_excludedFragments.set( Fragment::GetFragmentIndex( type ) );
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
			m_includedFragments.reset( Fragment::GetFragmentIndex( T::GetTypeStatic() ) );
			m_excludedFragments.reset( Fragment::GetFragmentIndex( T::GetTypeStatic() ) );
		}

		const std::bitset< Tag::c_maxTagsAmount >& GetRequiredTags() const
		{
			return m_includedTags;
		}

		const std::bitset< Fragment::c_maxFragmentsAmount >& GetRequiredFragments() const
		{
			return m_includedFragments;
		}

		const std::bitset< Tag::c_maxTagsAmount >& GetExcludedTags() const
		{
			return m_excludedTags;
		}

		const std::bitset< Fragment::c_maxFragmentsAmount >& GetExcludedFragments() const
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

		using VisitFunc = std::function< void( Archetype& ) >;
		void VisitArchetypes( ECSManager& ecsManager, const VisitFunc& visitFunc ) const;
		
		using VisitFuncWithCommands = std::function< void( Archetype&, DelayedCommands& commands ) >;
		void VisitArchetypes( ECSManager& ecsManager, const VisitFuncWithCommands& visitFunc ) const;

	private:
		std::bitset< Tag::c_maxTagsAmount > m_includedTags;
		std::bitset< Tag::c_maxTagsAmount > m_excludedTags;

		std::bitset< Fragment::c_maxFragmentsAmount > m_includedFragments;
		std::bitset< Fragment::c_maxFragmentsAmount > m_excludedFragments;
	};
}

