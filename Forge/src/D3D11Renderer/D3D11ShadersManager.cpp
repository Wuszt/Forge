#include "Fpch.h"
#include "D3D11ShadersManager.h"
#include "../Renderer/ShaderPack.h"
#include "../Core/DepotsContainer.h"

d3d11::D3D11ShadersManager::D3D11ShadersManager( const forge::DepotsContainer& depotsContainer, D3D11Device& device, D3D11RenderContext& context )
	: IShadersManager( depotsContainer )
	, m_device( device )
	, m_context( context )
{}

std::unique_ptr< renderer::IVertexShader > d3d11::D3D11ShadersManager::CreateVertexShader( const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines ) const
{
	std::string absolutePath;
	if( m_depotsContainer.TryToGetExistingFilePath( "Shaders\\" + path, absolutePath ) )
	{
		return std::make_unique< d3d11::D3D11VertexShader >( m_device, m_context, absolutePath, defines );
	}

	return nullptr;
}

std::unique_ptr< renderer::IPixelShader > d3d11::D3D11ShadersManager::CreatePixelShader( const std::string& path, forge::ArraySpan< const renderer::ShaderDefine > defines ) const
{
	std::string absolutePath;
	if( m_depotsContainer.TryToGetExistingFilePath( "Shaders\\" + path, absolutePath ) )
	{
		return std::make_unique< d3d11::D3D11PixelShader >( m_device, m_context, absolutePath, defines );
	}

	return nullptr;
}
