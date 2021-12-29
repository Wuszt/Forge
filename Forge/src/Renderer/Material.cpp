#include "Fpch.h"
#include "../Renderer/IRenderer.h"
#include "IShadersManager.h"

renderer::Material::Material( renderer::IRenderer& renderer, const Model& model, std::unique_ptr< ConstantBuffer >&& buffer, const std::string& vsPath, const std::string& psPath )
	: m_renderer( renderer )
{
	SetVertexShader( vsPath );
	SetPixelShader( psPath );
	m_pixelShader = renderer.GetShadersManager()->GetPixelShader( psPath );
	m_constantBuffer = std::move( buffer );
	m_inputLayout = renderer.CreateInputLayout( *m_vertexShader, *model.GetVertexBuffer() );
}

void renderer::Material::SetVertexShader( const std::string& path )
{
	m_vertexShader = m_renderer.GetShadersManager()->GetVertexShader( path );

#ifdef FORGE_DEBUGGING
	m_debugVertexShaderPath = path;
#endif
}

void renderer::Material::SetPixelShader( const std::string& path )
{
	m_pixelShader = m_renderer.GetShadersManager()->GetPixelShader( path );

#ifdef FORGE_DEBUGGING
	m_debugPixelShaderPath = path;
#endif
}

void renderer::Material::SetTexture( const std::string& path, Uint32 index )
{
	if( index >= GetTexturesAmount() )
	{
		m_textures.resize( index + 1u );
	}

	m_textures[ index ] = m_renderer.GetResourceManager().LoadTexture( path );
}
