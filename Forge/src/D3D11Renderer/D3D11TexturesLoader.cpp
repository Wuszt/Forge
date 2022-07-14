#include "Fpch.h"
#include "D3D11TexturesLoader.h"
#include "../../External/WICTextureLoader/WICTextureLoader11.h"
#include "D3D11Device.h"
#include "../Renderer/TextureAsset.h"

d3d11::D3D11TexturesLoader::D3D11TexturesLoader( const d3d11::D3D11Device& device, const D3D11RenderContext& context )
	: m_device( device )
	, m_context( context )
{}

std::vector< std::shared_ptr< forge::IAsset > > d3d11::D3D11TexturesLoader::LoadAssets( const std::string& path ) const
{
	std::wstring wPath = std::wstring( path.begin(), path.end() );
	ID3D11Resource* resource = nullptr;
	ID3D11ShaderResourceView* resourceView = nullptr;
	DirectX::CreateWICTextureFromFile( m_device.GetDevice(), wPath.c_str(), &resource, &resourceView );

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	resourceView->GetDesc( &srvDesc );

	resourceView->Release();

	return { std::make_shared< renderer::TextureAsset >( path, std::make_unique< d3d11::D3D11Texture >( m_device, m_context, *static_cast<ID3D11Texture2D*>( resource ), srvDesc.Format ) ) };
}

static const char* c_handledExceptions[] = { "jpg", "png" };
forge::ArraySpan< const char * > d3d11::D3D11TexturesLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}
