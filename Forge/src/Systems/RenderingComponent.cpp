#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/IRenderer.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/IShadersManager.h"

IMPLEMENT_TYPE( forge::RenderableFragment )
IMPLEMENT_TYPE( forge::RenderingComponent )
IMPLEMENT_TYPE( forge::DirtyRenderable );

forge::RenderingComponent::RenderingComponent() = default;
forge::RenderingComponent::~RenderingComponent() = default;

void forge::RenderingComponent::OnAttach( EngineInstance& engineInstance )
{
	DataComponent< forge::RenderableFragment >::OnAttach( engineInstance );
	GetDirtyRenderable() = renderer::Renderable( engineInstance.GetRenderer() );
	engineInstance.GetRenderer().AddRenderableECSFragment( engineInstance.GetECSManager(), engineInstance.GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
	m_onShadersClearCache = engineInstance.GetRenderer().GetShadersManager()->RegisterCacheClearingListener(
		[ this ]()
		{
			SetDirty();
		} );
}

void forge::RenderingComponent::LoadMeshAndMaterial( const std::string& path )
{
	GetDirtyRenderable().SetModel( GetOwner().GetEngineInstance().GetAssetsManager(), path );
}

void forge::RenderingComponent::SetDirty()
{
	GetOwner().GetEngineInstance().GetECSManager().AddTagToEntity< DirtyRenderable >( GetOwner().GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetOwner().GetObjectID() ) );
}
