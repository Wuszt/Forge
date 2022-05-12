#include "Fpch.h"
#include "IRenderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"

namespace renderer
{
	std::unique_ptr< IRenderer > IRenderer::CreateRenderer( const forge::DepotsContainer& depotsContainer, forge::IWindow& window, RendererType type )
	{
		std::unique_ptr< IRenderer > result = nullptr;

		switch( type )
		{
		case RendererType::D3D11:
			result = std::make_unique< d3d11::D3D11Renderer >( depotsContainer, window );
			break;
		default:
			FORGE_FATAL( "Unknown renderer type" );
			return nullptr;
		}

		result->Initialize();
		return result;
	}

	IRenderer::IRenderer( const forge::DepotsContainer& depotsContainer )
		: m_depotsContainer( depotsContainer )
	{}

	IRenderer::~IRenderer() = default;

	void IRenderer::Initialize()
	{
		m_resourcesManager = std::make_unique< ResourcesManager >( m_depotsContainer, *this );
	}

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
