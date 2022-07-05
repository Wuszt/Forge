#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/IRenderer.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "EngineInstance.h"

IMPLEMENT_TYPE( forge, RenderingComponentData )
IMPLEMENT_TYPE( forge, RenderingComponent )

forge::RenderingComponent::RenderingComponent() = default;
forge::RenderingComponent::~RenderingComponent() = default;

void forge::RenderingComponent::LoadMeshAndMaterial( const std::string& path )
{
	m_renderable = std::make_unique < renderer::Renderable >( GetOwner().GetEngineInstance().GetRenderer(), GetOwner().GetEngineInstance().GetAssetsManager(), path );
	GetData().m_renderable = m_renderable.get();
}
