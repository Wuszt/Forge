#pragma once
#include "Tag.h"

namespace ecs
{
	class Query
	{
	public:
		template< class T >
		void AddTagRequirement()
		{
			m_tags.set( T::GetTagIndex() );
		}

		template< class T >
		void AddFragmentRequirement()
		{
			m_fragments.set( T::GetFragmentIndex() );
		}

		const std::bitset< Tag::c_maxTagsAmount >& GetRequiredTags() const
		{
			return m_tags;
		}

		const std::bitset< Fragment::c_maxFragmentsAmount >& GetRequiredFragments() const
		{
			return m_fragments;
		}

		using VisitFunc = std::function< void(Archetype&) >;
		void VisitArchetypes( ECSManager& ecsManager, const VisitFunc& visitFunc ) const;

	private:
		std::bitset< Tag::c_maxTagsAmount > m_tags;
		std::bitset< Fragment::c_maxFragmentsAmount > m_fragments;
	};
}

