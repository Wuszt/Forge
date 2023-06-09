#include "Fpch.h"
#include "AnimationSystem.h"
#include "TimeSystem.h"
#include "../ECS/Query.h"
#include "RenderingComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#include "DebugSystem.h"
#endif

RTTI_IMPLEMENT_TYPE( systems::AnimationSystem );

void systems::AnimationSystem::OnInitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	InitializeDebuggable< systems::AnimationSystem >( GetEngineInstance() );
#endif
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, std::bind( &systems::AnimationSystem::Update, this ) );
}

void systems::AnimationSystem::Update()
{
	systems::TimeSystem& timeSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::TimeSystem >();

	ecs::Query query;
	query.AddFragmentRequirement< forge::RenderableFragment >( ecs::Query::RequirementType::Included );
	query.AddFragmentRequirement< forge::AnimationFragment >( ecs::Query::RequirementType::Included );

	query.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
			auto animationFragments = archetype.GetFragments< forge::AnimationFragment >();
			auto renderableFragments = archetype.GetFragments< forge::RenderableFragment >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				auto& animFragment = animationFragments[ i ];
				animFragment.m_currentTimeProgress += timeSystem.GetDeltaTime();
				animFragment.m_animation.Sample( animFragment.m_currentTimeProgress, true, m_temporaryTransforms );

				for ( Uint32 j = 0u; j < static_cast< Uint32 >( m_temporaryTransforms.size() ); ++j )
				{
					animFragment.m_cb->GetData().Transforms[ j ] = animFragment.m_bonesOffsets[ j ] * m_temporaryTransforms[ j ];
				}

				animFragment.m_cb->UpdateBuffer();
			}
		} );
}

#ifdef FORGE_IMGUI_ENABLED
void systems::AnimationSystem::OnRenderDebug()
{
	if ( ImGui::Begin( "Animation System" ) )
	{
		ecs::Query query;
		query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		query.AddFragmentRequirement< forge::AnimationFragment >( ecs::Query::RequirementType::Included );

		query.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
		    auto animationFragments = archetype.GetFragments< forge::AnimationFragment >();
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				if ( ImGui::TreeNodeEx( forge::String::Printf( "Entity: %u", static_cast< Uint32 >( archetype.GetEntityIDWithIndex( i ) ) ).c_str() ) )
				{
					const auto& animFragment = animationFragments[ i ];
					Matrix entityWorldMatrix = transformFragments[ i ].ToMatrix();
					for ( Uint32 j = 0u; j < static_cast< Uint32 >( m_temporaryTransforms.size() ); ++j )
					{
						Matrix boneMatrix = animFragment.m_bonesOffsets[ j ].AffineInverted() * animFragment.m_cb->GetData().Transforms[j] * entityWorldMatrix;

						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( boneMatrix.GetTranslation(), 5.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f), false, true, 0.0f);
					}

					ImGui::TreePop();
				}
			}

		} );
	}

	ImGui::End();
}
#endif

