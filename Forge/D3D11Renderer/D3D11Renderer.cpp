#include "Fpch.h"
#include "D3D11Renderer.h"
#include "D3D11Window.h"

D3D11Renderer::D3D11Renderer( Uint32 width, Uint32 height )
{
	m_window = std::make_unique< D3D11Window >( GetModuleHandle( NULL ), width, height );
}

D3D11Renderer::~D3D11Renderer() = default;
