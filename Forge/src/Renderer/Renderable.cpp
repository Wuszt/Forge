#include "Fpch.h"
#include "Renderable.h"
#include "IModelsLoader.h"

renderer::Renderable::Renderable( IRenderer& renderer, const std::string& path )
	: m_renderer( renderer )
{
	m_cbMesh.SetImpl( renderer.CreateConstantBufferImpl() );
	SetModel( path );
}

renderer::Renderable::~Renderable() = default;

void renderer::Renderable::SetModel( const std::string& path )
{
	auto modelMaterialPackage = m_renderer.GetResourceManager().LoadModel( path );
	m_model = modelMaterialPackage->m_model;

	std::string texturesFolderPath = path.substr( 0u, path.find_last_of( '\\' ) ) + "\\";

	for( auto& materialData : modelMaterialPackage->m_materialsData )
	{
		m_materials.emplace_back( std::make_unique< Material >( m_renderer, *m_model, m_renderer.CreateConstantBufferFromOther( *materialData.m_buffer ), "Uber.fx", "Uber.fx", renderer::RenderingPass::Overlay ) );

		if( !materialData.m_diffuseTextureName.empty() )
		{
			m_materials.back()->SetTexture( texturesFolderPath + materialData.m_diffuseTextureName, Material::TextureType::Diffuse );
		}

		if( !materialData.m_normalTextureName.empty() )
		{
			m_materials.back()->SetTexture( texturesFolderPath + materialData.m_normalTextureName, Material::TextureType::Normal );
		}

		if( !materialData.m_alphaTextureName.empty() )
		{
			m_materials.back()->SetTexture( texturesFolderPath + materialData.m_alphaTextureName, Material::TextureType::Alpha );
		}
	}

	if( modelMaterialPackage->m_materialsData.empty() )
	{
		auto constantBuffer = m_renderer.CreateConstantBuffer();
		constantBuffer->AddData( "diffuseColor", Vector4( 230.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f, 1.0f ) );
		constantBuffer->UpdateBuffer();
		m_materials.emplace_back( std::make_unique< Material >( m_renderer, *m_model, std::move( constantBuffer ), "Uber.fx", "Uber.fx", renderer::RenderingPass::Opaque ) );
		for( auto& shape : m_model->GetShapes() )
		{
			shape.m_materialIndex = 0u;
		}
	}
}
