#pragma once
#include "../ECS/Fragment.h"
#include "../GameEngine/IComponent.h"
#include "../Renderer/AnimationSetAsset.h"
#include "../Renderer/ConstantBuffer.h"

namespace renderer
{
	class SkeletonAsset;
}

namespace forge
{
	constexpr Uint32 c_maxBonesAmount = 70u;

	struct AnimationFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( AnimationFragment, ecs::Fragment );

		AnimationFragment();

		renderer::Animation m_animation;
		std::vector< Matrix > m_bonesOffsets = std::vector< Matrix >( c_maxBonesAmount );

		struct CB
		{
			Matrix Transforms[ c_maxBonesAmount ];
		};

		std::unique_ptr< renderer::StaticConstantBuffer< CB > > m_cb = std::make_unique<renderer::StaticConstantBuffer< CB >>();

		Float m_currentTimeProgress = 0.0f;
	};

	class AnimationComponent : public DataComponent< AnimationFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( AnimationComponent, forge::IDataComponent );
	public:
		using DataComponent::DataComponent;

		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData ) override;
		virtual void OnDetached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

		void SetAnimation( renderer::Animation animation );
		void SetSkeleton( const renderer::SkeletonAsset& skeleton );
	};
}

