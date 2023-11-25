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

		void RecalculateWorldTransform( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue );
		void RecalculateWorldTransform( ecs::EntityID id, forge::TransformFragment& transformFragment, ecs::CommandsQueue& commandsQueue );

		void RecalculateLocalTransform( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue );
		void RecalculateLocalTransform( ecs::EntityID id, forge::TransformChildFragment& transformChildFragment, ecs::CommandsQueue& commandsQueue );

		void MakeDirty( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue );
		void MakeDirty( ecs::EntityID id, ecs::CommandsQueue& commandsQueue );

		void OnReadingTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue );
		void OnReadingTransformFragment( ecs::MutableFragmentUntypedView fragment );

		void OnModifyingTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue );
		void OnModifyingTransformFragment( ecs::MutableFragmentUntypedView fragment );

		void OnReadingChildTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue );
		void OnReadingChildTransformFragment( ecs::MutableFragmentUntypedView fragment );

		void OnModifyingChildTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue );
		void OnModifyingChildTransformFragment( ecs::MutableFragmentUntypedView fragment );

		forge::CallbackToken m_updateToken;
		forge::CallbackToken m_modifyingArchetypeToken;
		forge::CallbackToken m_modifyingEntityToken;

		forge::CallbackToken m_modifyingChildArchetypeToken;
		forge::CallbackToken m_modifyingChildEntityToken;

		forge::CallbackToken m_readingArchetypeToken;
		forge::CallbackToken m_readingEntityToken;

		forge::CallbackToken m_readingChildArchetypeToken;
		forge::CallbackToken m_readingChildEntityToken;

		ecs::CommandsQueue* m_currentCommandsQueue = nullptr;
	};
}