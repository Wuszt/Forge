#include "Fpch.h"
#include "Renderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"
#include "TinyObjModelsLoader.h"
#include "../Core/AssetsManager.h"
#include "FBXLoader.h"
#include "../ECS/ECSManager.h"

RTTI_IMPLEMENT_TYPE( renderer::WireFrameTag );
RTTI_DECLARE_AND_IMPLEMENT_ENUM( renderer::FillMode,
	RTTI_REGISTER_ENUM_MEMBER( Solid );
	RTTI_REGISTER_ENUM_MEMBER( WireFrame );
);

RTTI_DECLARE_AND_IMPLEMENT_ENUM( renderer::RenderingPass,
	RTTI_REGISTER_ENUM_MEMBER( Opaque );
	RTTI_REGISTER_ENUM_MEMBER( Transparent );
);

namespace renderer
{
	std::unique_ptr< Renderer > Renderer::CreateRenderer( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::IWindow& window, RendererType type )
	{
		std::unique_ptr< Renderer > result = nullptr;

		switch( type )
		{
		case RendererType::D3D11:
			result = std::make_unique< d3d11::D3D11Renderer >( depotsContainer, assetsManager, window );
			break;
		default:
			FORGE_FATAL( "Unknown renderer type" );
			return nullptr;
		}

		return result;
	}

	Renderer::Renderer( forge::AssetsManager& assetsManager )
	{
		assetsManager.AddAssetsLoader< TinyObjModelsLoader >( *this );
		assetsManager.AddAssetsLoader< renderer::FBXLoader >( *this );
	}

	Renderer::~Renderer() = default;

	void Renderer::Draw( const renderer::Renderable& renderable )
	{
		renderable.GetModel().GetVertexBuffer()->Set();
		renderable.GetCBMesh().SetVS( VSConstantBufferType::Mesh );
		renderable.GetCBMesh().SetPS( PSConstantBufferType::Mesh );

		for( Uint32 i = 0; i < renderable.GetModel().GetShapes().size(); ++i )
		{
			renderable.GetModel().GetShapes()[ i ].m_indexBuffer->Set( 0 );

			auto& material = renderable.GetMaterials()[ i ];

			material->GetVertexShader()->GetMainShader()->Set();
			material->GetPixelShader()->GetMainShader()->Set();
			material->GetInputLayout()->Set();

			std::vector< const IShaderResourceView* > srvs;
			srvs.reserve( material->GetTextures().GetSize() );

			for ( auto texture : material->GetTextures() )
			{
				srvs.emplace_back( texture ? texture->GetShaderResourceView() : nullptr );
			}

			SetShaderResourceViews( srvs );

			renderable.GetMaterials()[ i ]->GetConstantBuffer()->SetVS( renderer::VSConstantBufferType::Material );

			GetContext()->Draw( renderable.GetModel().GetShapes()[ i ].m_indexBuffer->GetIndicesAmount(), 0 );
		}
	}
}
