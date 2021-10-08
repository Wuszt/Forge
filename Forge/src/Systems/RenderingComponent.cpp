#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/IRenderer.h"
#include "../Renderer/IMesh.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"

void forge::RenderingComponent::OnAttach( EngineInstance& engineInstance )
{
	DataComponent< RenderingComponentData >::OnAttach( engineInstance );
	m_renderable = std::make_unique< renderer::Renderable >( engineInstance.GetRenderer() );

	GetData().m_renderable = m_renderable.get();
}
