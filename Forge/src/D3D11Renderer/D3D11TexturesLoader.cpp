#include "Fpch.h"
#include "D3D11TexturesLoader.h"
#include "../../External/WICTextureLoader/WICTextureLoader11.h"
#include "D3D11Device.h"

d3d11::D3D11TexturesLoader::D3D11TexturesLoader( const d3d11::D3D11Device& device, const D3D11RenderContext& context )
	: m_device( device )
	, m_context( context )
{}

std::shared_ptr< renderer::ITexture > d3d11::D3D11TexturesLoader::LoadTexture( const std::string& path )
{
	const std::string finalPath = "Resources/Textures/" + path;

	std::wstring wPath = L"Resources/Textures/" + std::wstring( path.begin(), path.end() );
	ID3D11Resource* resource = nullptr;
	ID3D11ShaderResourceView* resourceView = nullptr;
	DirectX::CreateWICTextureFromFile( m_device.GetDevice(), wPath.c_str(), &resource, &resourceView );

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	resourceView->GetDesc( &srvDesc );

	resourceView->Release();

	return std::make_unique< d3d11::D3D11Texture >( m_device, m_context, *static_cast< ID3D11Texture2D* >( resource ), srvDesc.Format );
}
