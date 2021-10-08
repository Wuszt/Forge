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

	void IRenderer::Draw( const renderer::Renderable& renderable )
	{
		renderable.GetMesh()->GetVertexBuffer()->Set();
		renderable.GetMesh()->GetIndexBuffer()->Set( 0 );

		renderable.GetMaterial()->GetVertexShader()->Set();
		renderable.GetMaterial()->GetPixelShader()->Set();

		renderable.GetInputLayout()->Set();

		renderable.GetConstantBuffer()->SetVS( renderer::VSConstantBufferType::Material );

		GetContext()->Draw( renderable.GetMesh()->GetIndexBuffer()->GetIndicesAmount(), 0 );
	}

}
