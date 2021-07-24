#include "Fpch.h"
#include "IRenderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"

std::unique_ptr< IRenderer > IRenderer::CreateRenderer( const IWindow& window, RendererType type )
{
	switch( type )
	{
	case RendererType::D3D11:
		return std::make_unique< D3D11Renderer >( window );
	default:
		FORGE_FATAL( "Unknown renderer type" );
		return nullptr;
	}
}
