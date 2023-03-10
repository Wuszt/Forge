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
		DECLARE_STRUCT( AnimationFragment, ecs::Fragment );

		AnimationFragment();

		renderer::Animation m_animation;
		Matrix m_bonesOffsets[ c_maxBonesAmount ];

		struct CB
		{
			Matrix Transforms[ c_maxBonesAmount ];
		};

		renderer::StaticConstantBuffer< CB > m_cb;

		Float m_currentTimeProgress = 0.0f;
	};

	class AnimationComponent : public DataComponent< AnimationFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( AnimationComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;

		virtual void OnAttach( EngineInstance& engineInstance ) override;
		virtual void OnDetach( EngineInstance& engineInstance ) override;

		void SetAnimation( renderer::Animation animation );
		void SetSkeleton( const renderer::SkeletonAsset& skeleton );
	};
}

