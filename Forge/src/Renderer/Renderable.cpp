#include "Fpch.h"
#include "Renderable.h"
#include "ModelAsset.h"
#include "TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "ConstantBuffer.h"

RTTI_IMPLEMENT_TYPE( renderer::Renderable,
	RTTI_REGISTER_PROPERTY( m_materials );
	RTTI_REGISTER_PROPERTY( m_fillMode );
);

renderer::Renderable::Renderable( Renderer& renderer )
	: m_renderer( &renderer )
{
	m_cbMesh.SetImpl( renderer.CreateConstantBufferImpl() );
}

renderer::Renderable& renderer::Renderable::operator=( Renderable&& ) = default;

renderer::Renderable::Renderable() = default;

renderer::Renderable::~Renderable() = default;

renderer::Renderable::Renderable( Renderable&& ) = default;

void renderer::Renderable::SetModel( forge::AssetsManager& assetsManager, const forge::Path& path )
{
	auto modelAsset = assetsManager.GetAsset< renderer::ModelAsset >( path );
	m_model = modelAsset->GetModel();

	const forge::Path texturesFolderPath = path.GetFolder();

	m_materials.clear();
	for( auto& materialData : modelAsset->GetMaterialsData() )
	{
		m_materials.emplace_back( std::make_unique< Material >( *m_renderer, *m_model, m_renderer->CreateConstantBufferFromOther( *materialData.m_buffer ), forge::Path( "Uber.fx" ), forge::Path( "Uber.fx" ), renderer::RenderingPass::Opaque ) );

		if( !materialData.m_diffuseTextureName.empty() )
		{
			const forge::Path texturePath( texturesFolderPath.AsString() + materialData.m_diffuseTextureName );
			if( auto textureAsset = assetsManager.GetAsset< renderer::TextureAsset >( texturePath ) )
			{
				m_materials.back()->SetTexture( textureAsset->GetTexture(), Material::TextureType::Diffuse );
			}
			else
			{
				FORGE_LOG_WARNING( "Can't load texture %s", texturePath.Get() );
			}
		}

		if( !materialData.m_normalTextureName.empty() )
		{
			const forge::Path texturePath( texturesFolderPath.AsString() + materialData.m_normalTextureName );
			if( auto textureAsset = assetsManager.GetAsset< renderer::TextureAsset >( texturePath ) )
			{
				m_materials.back()->SetTexture( textureAsset->GetTexture(), Material::TextureType::Normal );
			}
			else
			{
				FORGE_LOG_WARNING( "Can't load texture %s", texturePath.Get() );
			}
		}

		if( !materialData.m_alphaTextureName.empty() )
		{
			const forge::Path texturePath( texturesFolderPath.AsString() + materialData.m_alphaTextureName );
			if( auto textureAsset = assetsManager.GetAsset< renderer::TextureAsset >( texturePath ) )
			{
				m_materials.back()->SetTexture( textureAsset->GetTexture(), Material::TextureType::Alpha );
			}
			else
			{
				FORGE_LOG_WARNING( "Can't load texture %s", texturePath.Get() );
			}
		}
	}

	if( modelAsset->GetMaterialsData().empty() )
	{
		auto constantBuffer = m_renderer->CreateConstantBuffer();
		constantBuffer->AddData( "diffuseColor", LinearColor( 230.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f ) );
		constantBuffer->UpdateBuffer();
		m_materials.emplace_back( std::make_unique< Material >( *m_renderer, *m_model, std::move( constantBuffer ), forge::Path( "Uber.fx" ), forge::Path( "Uber.fx" ), renderer::RenderingPass::Opaque ) );
		for( auto& shape : m_model->GetShapes() )
		{
			shape.m_materialIndex = 0u;
		}
	}
}
