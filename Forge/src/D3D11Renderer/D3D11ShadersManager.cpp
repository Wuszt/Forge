#include "Fpch.h"
#include "D3D11ShadersManager.h"

d3d11::D3D11ShadersManager::D3D11ShadersManager( D3D11Device& device, D3D11RenderContext& context )
	: m_device( device )
	, m_context( context )
{}

std::unique_ptr< const renderer::IVertexShader > d3d11::D3D11ShadersManager::CreateVertexShader( const std::string& path ) const
{
	return std::make_unique< d3d11::D3D11VertexShader >( m_device, m_context, "Shaders\\" + path );
}

std::unique_ptr< const renderer::IPixelShader > d3d11::D3D11ShadersManager::CreatePixelShader( const std::string& path ) const
{
	return std::make_unique< d3d11::D3D11PixelShader >( m_device, m_context, "Shaders\\" + path );
}
