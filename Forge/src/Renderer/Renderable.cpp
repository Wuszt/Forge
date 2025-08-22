#include "Fpch.h"
#include "Renderable.h"
#include "ModelAsset.h"
#include "TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "ConstantBuffer.h"

RTTI_IMPLEMENT_TYPE( renderer::Renderable,
	RTTI_REGISTER_PROPERTY( m_materials, RTTI_ADD_METADATA( "Editable" ) );
	RTTI_REGISTER_PROPERTY( m_fillMode, RTTI_ADD_METADATA( "Editable" ) );
);

renderer::Renderable::Renderable() = default;
renderer::Renderable::~Renderable() = default;

renderer::Renderable::Renderable( Renderable&& ) = default;
renderer::Renderable& renderer::Renderable::operator=( Renderable&& ) = default;

void renderer::Renderable::Initialize( Renderer& renderer )
{
	m_cbMesh.SetImpl( renderer.CreateConstantBufferImpl() );
}

void renderer::Renderable::SetModel( Model&& model )
{
	m_model = std::make_shared< Model >( std::move( model ) );
}

void renderer::Renderable::SetModel( Renderer& renderer, forge::AssetsManager& assetsManager, const forge::Path& path )
{
	auto modelAsset = assetsManager.GetAsset< renderer::ModelAsset >( path );
	m_model = modelAsset->GetModel();

	const forge::Path texturesFolderPath = path.GetFolder();

	m_materials.clear();
	for( auto& materialData : modelAsset->GetMaterialsData() )
	{
		m_materials.emplace_back( std::make_unique< Material >( renderer.CreateConstantBufferFromOther( *materialData.m_buffer ), forge::Path( "Uber.fx" ), forge::Path( "Uber.fx" ), renderer::RenderingPass::Opaque ) );
		m_materials.back()->Initialize( renderer, *m_model );

		if( !materialData.m_diffuseTextureName.empty() )
		{
			const forge::Path texturePath( texturesFolderPath.AsString() + materialData.m_diffuseTextureName );
			if( auto textureAsset = assetsManager.GetAsset< renderer::TextureAsset >( texturePath ) )
			{
				m_materials.back()->SetTexture( renderer, textureAsset->GetTexture(), Material::TextureType::Diffuse );
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
				m_materials.back()->SetTexture( renderer, textureAsset->GetTexture(), Material::TextureType::Normal );
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
				m_materials.back()->SetTexture( renderer, textureAsset->GetTexture(), Material::TextureType::Alpha );
			}
			else
			{
				FORGE_LOG_WARNING( "Can't load texture %s", texturePath.Get() );
			}
		}
	}

	if( modelAsset->GetMaterialsData().empty() )
	{
		SetDefaultMaterial( renderer );
	}
}

void renderer::Renderable::SetDefaultMaterial( Renderer& renderer )
{
	auto constantBuffer = renderer.CreateConstantBuffer();
	constantBuffer->AddData( "diffuseColor", LinearColor( 230.0f / 255.0f, 128.0f / 255.0f, 255.0f / 255.0f ) );
	constantBuffer->UpdateBuffer();

	m_materials.clear();
	m_materials.emplace_back( std::make_unique< Material >( std::move( constantBuffer ), forge::Path( "Uber.fx" ), forge::Path( "Uber.fx" ), renderer::RenderingPass::Opaque ) );
	m_materials.back()->Initialize( renderer, *m_model );
	for ( auto& shape : m_model->GetShapes() )
	{
		shape.m_materialIndex = 0u;
	}
}