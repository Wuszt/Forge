#include "Fpch.h"
#include "IRenderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"

namespace renderer
{
	std::unique_ptr< IRenderer > IRenderer::CreateRenderer( forge::IWindow& window, RendererType type )
	{
		switch( type )
		{
		case RendererType::D3D11:
			return std::make_unique< d3d11::D3D11Renderer >( window );
		default:
			FORGE_FATAL( "Unknown renderer type" );
			return nullptr;
		}
	}

	IRenderer::IRenderer()
	{
		m_resourcesManager = std::make_unique< ResourcesManager >( *this );
	}

	IRenderer::~IRenderer() = default;

	void IRenderer::Draw( const renderer::Renderable& renderable )
	{
		renderable.GetModel().GetVertexBuffer()->Set();

		for( Uint32 i = 0; renderable.GetModel().GetShapes().size(); ++i )
		{
			renderable.GetModel().GetShapes()[ i ].m_indexBuffer->Set( 0 );

			renderable.GetMaterials()[ i ].GetVertexShader()->Set();
			renderable.GetMaterials()[ i ].GetPixelShader()->Set();

			renderable.GetMaterials()[ i ].GetInputLayout()->Set();

			renderable.GetMaterials()[ i ].GetConstantBuffer()->SetVS( renderer::VSConstantBufferType::Material );

			GetContext()->Draw( renderable.GetModel().GetShapes()[ i ].m_indexBuffer->GetIndicesAmount(), 0 );
		}
	}
}
