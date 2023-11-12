#pragma once
#include "TransformComponent.h"

namespace systems
{
	class TransformSystem : public ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( TransformSystem, systems::ISystem );

	private:
		virtual void OnInitialize();
		void Update();
		void OnBeforeModifyingTransforms( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue );
		void OnBeforeModifyingTransform( ecs::EntityID id );

		forge::CallbackToken m_updateToken;
		forge::CallbackToken m_modifyingArchetypeToken;
		forge::CallbackToken m_modifyingEntityToken;
		forge::CallbackToken m_postCommandQueueExecutionToken;
	};
}