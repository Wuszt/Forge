#include "Fpch.h"
#include "Renderable.h"

renderer::Renderable::Renderable( IRenderer& renderer )
	: m_renderer( renderer )
{
	m_cbMesh.SetImpl( renderer.CreateConstantBufferImpl() );
}

renderer::Renderable::~Renderable() = default;

void renderer::Renderable::UpdateInputLayout()
{
	m_inputLayout = m_renderer.CreateInputLayout( *m_material->GetVertexShader(), *m_mesh->GetVertexBuffer() );
}
