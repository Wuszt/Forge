#include "Fpch.h"
#include "IRenderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"
#include "TinyObjModelsLoader.h"

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
		m_modelsLoader = std::make_unique< TinyObjModelsLoader >( *this );
	}

	IRenderer::~IRenderer() = default;

	void IRenderer::Draw( const renderer::Renderable& renderable )
	{
		renderable.GetModel().GetVertexBuffer()->Set();
		renderable.GetModel().GetShapes()[ 0 ].m_indexBuffer->Set( 0 );

		renderable.GetMaterials()[ 0 ].GetVertexShader()->Set();
		renderable.GetMaterials()[ 0 ].GetPixelShader()->Set();

		renderable.GetMaterials()[ 0 ].GetInputLayout()->Set();

		renderable.GetMaterials()[ 0 ].GetConstantBuffer()->SetVS( renderer::VSConstantBufferType::Material );

		GetContext()->Draw( renderable.GetModel().GetShapes()[ 0 ].m_indexBuffer->GetIndicesAmount(), 0 );
	}

	renderer::IModelsLoader& IRenderer::GetModelsLoader() const
	{
		return *m_modelsLoader;
	}

}
