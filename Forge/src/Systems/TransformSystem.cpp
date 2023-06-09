#include "Fpch.h"
#include "TransformSystem.h"
#include "../ECS/Query.h"

RTTI_IMPLEMENT_TYPE( systems::TransformSystem );

void systems::TransformSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameEnd, std::bind( &systems::TransformSystem::Update, this ) );
}

void systems::TransformSystem::Update()
{
	for ( auto& archetype : GetEngineInstance().GetECSManager().GetArchetypes() )
	{
		if ( archetype->GetArchetypeID().ContainsFragment< forge::PreviousFrameTransformFragment >() )
		{
			archetype->RemoveFragmentType< forge::PreviousFrameTransformFragment >();
		}

		if ( archetype->GetArchetypeID().ContainsFragment< forge::PreviousFrameScaleFragment >() )
		{
			archetype->RemoveFragmentType< forge::PreviousFrameScaleFragment >();
		}
	}
}
