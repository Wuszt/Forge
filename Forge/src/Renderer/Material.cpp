#include "Fpch.h"
#include "../Renderer/IRenderer.h"
#include "../Renderer/IShader.h"

renderer::Material::Material( renderer::IRenderer& renderer, const Model& model, std::unique_ptr< ConstantBuffer >&& buffer, const std::string& vsPath, const std::string& psPath, renderer::RenderingPass renderingPass )
	: m_renderer( renderer )
{
	SetShaders( vsPath, psPath, renderingPass );
	m_constantBuffer = std::move( buffer );
	m_inputLayout = renderer.CreateInputLayout( *m_vertexShader->GetMainShader(), *model.GetVertexBuffer() );

	m_onShadersClearCache = renderer.GetShadersManager()->RegisterCacheClearingListener(
	[ this ]()
	{
		SetShaders( m_vertexShaderPath, m_pixelShaderPath, m_renderingPass );
	} );
}

void renderer::Material::SetShaders( const std::string& vsPath, const std::string& psPath, renderer::RenderingPass renderingPass )
{
	std::vector< renderer::ShaderDefine > defines;

	if( m_textures[ static_cast< Uint32 >( TextureType::Diffuse ) ] )
	{
		defines.push_back( { "__DIFFUSE_TEXTURE__" } );
	}

	if( m_textures[ static_cast< Uint32 >( TextureType::Normal ) ] )
	{
		defines.push_back( { "__NORMAL_TEXTURE__" } );
	}

	if( m_textures[ static_cast< Uint32 >( TextureType::Alpha ) ] )
	{
		defines.push_back( { "__ALPHA_TEXTURE__" } );
	}

	m_vertexShader = m_renderer.GetShadersManager()->GetVertexShader( vsPath, defines, true );
	m_vertexShaderPath = vsPath;

	m_pixelShader = m_renderer.GetShadersManager()->GetPixelShader( psPath, defines, true );
	m_pixelShaderPath = psPath;

	m_renderingPass = renderingPass;
}

void renderer::Material::SetRenderingPass( renderer::RenderingPass renderingPass )
{
	SetShaders( m_vertexShaderPath, m_pixelShaderPath, renderingPass );
}

void renderer::Material::SetTexture( std::shared_ptr< const ITexture > texture, Material::TextureType textureType )
{
	m_textures[ static_cast< Uint32 >( textureType) ] = texture;
	SetShaders( m_vertexShaderPath, m_pixelShaderPath, m_renderingPass );
}
