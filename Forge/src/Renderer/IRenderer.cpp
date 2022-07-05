#include "Fpch.h"
#include "IRenderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"
#include "TinyObjModelsLoader.h"
#include "../Core/AssetsManager.h"

namespace renderer
{
	std::unique_ptr< IRenderer > IRenderer::CreateRenderer( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::IWindow& window, RendererType type )
	{
		std::unique_ptr< IRenderer > result = nullptr;

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

	IRenderer::IRenderer( forge::AssetsManager& assetsManager )
	{
		assetsManager.AddAssetsLoader< TinyObjModelsLoader >( *this );
	}

	IRenderer::~IRenderer() = default;

	void IRenderer::Draw( const renderer::Renderable& renderable )
	{
		renderable.GetModel().GetVertexBuffer()->Set();

		for( Uint32 i = 0; renderable.GetModel().GetShapes().size(); ++i )
		{
			renderable.GetModel().GetShapes()[ i ].m_indexBuffer->Set( 0 );

			renderable.GetMaterials()[ i ]->GetVertexShader()->GetMainShader()->Set();
			renderable.GetMaterials()[ i ]->GetPixelShader()->GetMainShader()->Set();

			renderable.GetMaterials()[ i ]->GetInputLayout()->Set();

			renderable.GetMaterials()[ i ]->GetConstantBuffer()->SetVS( renderer::VSConstantBufferType::Material );

			GetContext()->Draw( renderable.GetModel().GetShapes()[ i ].m_indexBuffer->GetIndicesAmount(), 0 );
		}
	}
}
