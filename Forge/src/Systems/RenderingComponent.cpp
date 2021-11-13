#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/IRenderer.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/IModelsLoader.h"

void forge::RenderingComponent::LoadMeshAndMaterial( const std::string& path )
{
	m_renderable = std::make_unique < renderer::Renderable >( GetOwner().GetEngineInstance().GetRenderer(), path );
	GetData().m_renderable = m_renderable.get();
}
