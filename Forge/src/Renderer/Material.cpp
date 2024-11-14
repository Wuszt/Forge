#include "Fpch.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/IShader.h"
#include "ConstantBuffer.h"
#include "TextureAsset.h"

RTTI_IMPLEMENT_TYPE( renderer::Material,
	RTTI_REGISTER_PROPERTY( m_constantBuffer, RTTI_ADD_METADATA( "Editable" ) );
	RTTI_REGISTER_PROPERTY( m_renderingPass, RTTI_ADD_METADATA( "Editable" ) );
);

RTTI_DECLARE_AND_IMPLEMENT_ENUM( renderer::Material::TextureType,
	RTTI_REGISTER_ENUM_MEMBER( Diffuse );
	RTTI_REGISTER_ENUM_MEMBER( Normal );
	RTTI_REGISTER_ENUM_MEMBER( Alpha );
);

std::vector< renderer::ShaderDefine > ConstructShaderDefines( const renderer::Model& model )
{
	std::vector< renderer::ShaderDefine > result;

	auto semanticDescriptions = model.GetVertexBuffer()->GetSemanticDescriptions();

	for( const auto& desc : semanticDescriptions )
	{
		result.emplace_back( renderer::ShaderDefine{ std::string( "__VERTEX_INPUT_" ) + desc.m_name + std::string( "_AMOUNT__" ) , std::to_string( desc.m_amount ) });
	}

	return result;
}

renderer::Material::Material( std::unique_ptr< ConstantBuffer >&& buffer, forge::Path vsPath, forge::Path psPath, renderer::RenderingPass renderingPass )
	: m_vertexShaderPath( std::move( vsPath ) )
	, m_pixelShaderPath( std::move( psPath ) )
	, m_renderingPass( renderingPass )
	, m_constantBuffer( std::move( buffer ) )
{}

void renderer::Material::Initialize( renderer::Renderer& renderer, const Model& model )
{
	m_shaderDefines = ConstructShaderDefines( model );
	SetShaders( renderer, m_vertexShaderPath, m_pixelShaderPath );
	m_inputLayout = renderer.CreateInputLayout( *m_vertexShader->GetMainShader(), *model.GetVertexBuffer() );
	m_onShadersClearCache = renderer.GetShadersManager()->RegisterCacheClearingListener(
		[ this, rendererPtr = &renderer ]()
		{
			SetShaders( *rendererPtr, m_vertexShaderPath, m_pixelShaderPath );
		} );
}

renderer::Material::Material() = default;
renderer::Material::Material( Material&& ) = default;
renderer::Material::~Material() = default;

void renderer::Material::SetShaders( renderer::Renderer& renderer, const forge::Path& vsPath, const forge::Path& psPath )
{
	std::vector< renderer::ShaderDefine > defines = m_shaderDefines;

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

	m_vertexShader = renderer.GetShadersManager()->GetVertexShader( vsPath, defines, true );
	m_vertexShaderPath = vsPath;

	m_pixelShader = renderer.GetShadersManager()->GetPixelShader( psPath, defines, true );
	m_pixelShaderPath = psPath;
}

void renderer::Material::SetRenderingPass( renderer::RenderingPass renderingPass )
{
	m_renderingPass = renderingPass;
}

void renderer::Material::SetTexture( renderer::Renderer& renderer, std::shared_ptr< const ITexture > texture, Material::TextureType textureType )
{
	m_textures[ static_cast< Uint32 >( textureType) ] = texture;
	SetShaders( renderer, m_vertexShaderPath, m_pixelShaderPath );
}
