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

		template< class T >
		void AddTagRequirement( ecs::Query::RequirementType requirement )
		{
			if( requirement == RequirementType::Included )
			{
				FORGE_ASSERT( !m_excludedTags.test( T::GetTagIndex() ) );
				m_includedTags.set( T::GetTagIndex() );
			}
			else
			{
				FORGE_ASSERT( !m_includedTags.test( T::GetTagIndex() ) );
				m_excludedTags.set( T::GetTagIndex() );
			}
		}

		template< class T >
		void RemoveTagRequirement()
		{
			m_includedTags.reset( T::GetTagIndex() );
			m_excludedTags.reset( T::GetTagIndex() );
		}

		template< class T >
		void AddFragmentRequirement( ecs::Query::RequirementType requirement )
		{
			if ( requirement == RequirementType::Included )
			{
				FORGE_ASSERT( !m_excludedFragments.test( T::GetFragmentIndex() ) );
				m_includedFragments.set( T::GetFragmentIndex() );
			}
			else
			{
				FORGE_ASSERT( !m_includedFragments.test( T::GetFragmentIndex() ) );
				m_excludedFragments.set( T::GetFragmentIndex() );
			}
		}

		template< class T >
		void RemoveFragmentRequirement()
		{
			m_includedFragments.reset( T::GetFragmentIndex() );
			m_excludedFragments.reset( T::GetFragmentIndex() );
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

