#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/ShadersManager.h"

IMPLEMENT_TYPE( forge::RenderableFragment )
IMPLEMENT_TYPE( forge::RenderingComponent )
IMPLEMENT_TYPE( forge::DirtyRenderable );
IMPLEMENT_TYPE( forge::IgnoresLights );
IMPLEMENT_TYPE( forge::DrawAsOverlay );

void forge::RenderingComponent::OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	DataComponent< RenderableFragment >::OnAttaching( engineInstance, commandsQueue );
	commandsQueue.AddFragment( engineInstance.GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ), engineInstance.GetRenderer().GetECSFragmentType() );
}

void forge::RenderingComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	PC_SCOPE_FUNC();

	DataComponent< forge::RenderableFragment >::OnAttached( engineInstance, commandsQueue );
	GetDirtyRenderable() = renderer::Renderable( engineInstance.GetRenderer() );
	m_onShadersClearCache = engineInstance.GetRenderer().GetShadersManager()->RegisterCacheClearingListener(
		[ this ]()
		{
			SetDirty();
		} );
}

void forge::RenderingComponent::LoadMeshAndMaterial( const std::string& path )
{
	PC_SCOPE_FUNC();

	GetDirtyRenderable().SetModel( GetOwner().GetEngineInstance().GetAssetsManager(), path );
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
	Bool hasTag = ecsManager.GetEntityArchetype( entityId )->GetArchetypeID().ContainsTag< T >();
	if ( add && !hasTag )
	{
		ecsManager.AddTagToEntity< T >( entityId );
	}
	else if ( !add && hasTag )
	{
		ecsManager.RemoveTagFromEntity< T >( entityId );
	}
}

void forge::RenderingComponent::SetInteractingWithLight( bool enabled )
{
	AddOrRemoveTag< forge::IgnoresLights >( GetOwner(), !enabled );
}

void forge::RenderingComponent::SetDrawAsOverlayEnabled( bool enabled )
{
	AddOrRemoveTag< forge::DrawAsOverlay >( GetOwner(), enabled );
}
