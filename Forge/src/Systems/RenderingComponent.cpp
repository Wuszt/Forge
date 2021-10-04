#include "Fpch.h"
#include "../Renderer/ConstantBuffer.h"
#include "RenderingComponent.h"
#include "../Renderer/IRenderer.h"

void forge::RenderingComponent::OnAttach( EngineInstance& engineInstance )
{
	DataComponent< RenderingComponentData >::OnAttach( engineInstance );
	m_constantBuffer = engineInstance.GetRenderer().CreateConstantBuffer();
	GetData().m_constantBufferImplementation = m_constantBuffer->GetImpl();
}
