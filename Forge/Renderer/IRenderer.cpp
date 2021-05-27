#include "Fpch.h"
#include "IRenderer.h"
#include "../D3D11Renderer/D3D11Renderer.h"

std::unique_ptr< IRenderer > IRenderer::CreateRenderer( Uint32 width, Uint32 height )
{
	return std::make_unique< D3D11Renderer >( width, height );
}
