#include "Fpch.h"
#include "D3D11TexturesLoader.h"
#include "../../External/WICTextureLoader/WICTextureLoader11.h"
#include "D3D11Device.h"

d3d11::D3D11TexturesLoader::D3D11TexturesLoader( const d3d11::D3D11Device& device )
	: m_device( device )
{}

std::shared_ptr< renderer::ITexture > d3d11::D3D11TexturesLoader::LoadTexture( const std::string& path )
{
	const std::string finalPath = "Resources/Textures/" + path;

	std::wstring wPath = L"Resources/Textures/" + std::wstring( path.begin(), path.end() );
	ID3D11Resource* resource = nullptr;
	ID3D11ShaderResourceView* resourceView = nullptr;
	DirectX::CreateWICTextureFromFile( m_device.GetDevice(), wPath.c_str(), &resource, &resourceView );
	return std::make_unique< d3d11::D3D11Texture >( m_device, static_cast< ID3D11Texture2D* >( resource ), std::make_unique< D3D11ShaderResourceView >( resourceView ) );
}
