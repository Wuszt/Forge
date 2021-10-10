#include "Fpch.h"
#include "../Renderer/IRenderer.h"
#include "IShadersManager.h"

renderer::Material::Material( renderer::IRenderer& renderer, const std::string& vsPath, const std::string& psPath )
{
	m_vertexShader = renderer.GetShadersManager()->GetVertexShader( vsPath );
	m_pixelShader = renderer.GetShadersManager()->GetPixelShader( psPath );
	m_constantBuffer = renderer.CreateConstantBuffer();
}
