#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/ShadersManager.h"
#include "../GameEngine/RenderingManager.h"

RTTI_IMPLEMENT_TYPE( forge::RenderableFragment,
	RTTI_REGISTER_PROPERTY( m_renderable )
);

RTTI_IMPLEMENT_TYPE( forge::RenderingComponent )
RTTI_IMPLEMENT_TYPE( forge::DirtyRenderable );
RTTI_IMPLEMENT_TYPE( forge::IgnoresLights );
RTTI_IMPLEMENT_TYPE( forge::DrawAsOverlay );

void forge::RenderingComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	PC_SCOPE_FUNC();

	DataComponent< forge::RenderableFragment >::OnAttached( engineInstance, commandsQueue );
	GetDirtyData()->m_renderable = renderer::Renderable(engineInstance.GetRenderingManager().GetRenderer());
	m_onShadersClearCache = engineInstance.GetRenderingManager().GetRenderer().GetShadersManager()->RegisterCacheClearingListener(
		[ this ]()
		{
			SetDirty();
		} );
}

void forge::RenderingComponent::OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	Super::OnDetaching( engineInstance, commandsQueue );

	const auto entityID = engineInstance.GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() );
	
	const auto& renderingFragmentType = engineInstance.GetRenderingManager().GetRenderer().GetECSFragmentType();
	if ( engineInstance.GetECSManager().GetEntityArchetypeId( entityID ).ContainsFragment( renderingFragmentType ) )
	{
		commandsQueue.RemoveFragment( engineInstance.GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ), engineInstance.GetRenderingManager().GetRenderer().GetECSFragmentType() );
	}
}

void forge::RenderingComponent::LoadMeshAndMaterial( const forge::Path& path )
{
	PC_SCOPE_FUNC();

	GetDirtyData()->m_renderable.SetModel( GetOwner().GetEngineInstance().GetAssetsManager(), path );
	auto& engineInstance = GetOwner().GetEngineInstance();
	engineInstance.GetECSManager().AddFragmentToEntity( engineInstance.GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ), engineInstance.GetRenderingManager().GetRenderer().GetECSFragmentType() );
}

void forge::RenderingComponent::SetDirty()
{
	GetOwner().GetEngineInstance().GetECSManager().AddTagToEntity< DirtyRenderable >( GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
}

template< class T >
static void AddOrRemoveTag( forge::Object& owner, Bool add )
{
	auto& ecsManager = owner.GetEngineInstance().GetECSManager();
	auto entityId = owner.GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( owner.GetObjectID() );
	const Bool hasTag = ecsManager.GetEntityArchetypeId( entityId ).ContainsTag< T >();
	if ( add && !hasTag )
	{
		ecsManager.AddTagToEntity< T >( entityId );
	}
	else if ( !add && hasTag )
	{
		ecsManager.RemoveTagFromEntity< T >( entityId );
	}
}

void forge::RenderingComponent::SetInteractingWithLight( Bool enabled )
{
	AddOrRemoveTag< forge::IgnoresLights >( GetOwner(), !enabled );
}

void forge::RenderingComponent::SetDrawAsOverlayEnabled( Bool enabled )
{
	AddOrRemoveTag< forge::DrawAsOverlay >( GetOwner(), enabled );
}
