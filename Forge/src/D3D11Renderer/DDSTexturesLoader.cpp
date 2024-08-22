#include "Fpch.h"
#include "DDSTexturesLoader.h"
#include "../../External/DDSTextureLoader/DDSTextureLoader11.h"
#include "D3D11Device.h"
#include "../Renderer/TextureAsset.h"
#include "../Core/Path.h"

d3d11::DDSTexturesLoader::DDSTexturesLoader( const D3D11Device& device, const D3D11RenderContext& context )
	: m_device( device )
	, m_context( context )
{}

std::vector< std::shared_ptr< forge::IAsset > > d3d11::DDSTexturesLoader::LoadAssets( const forge::Path& path ) const
{
	std::wstring wPath = std::wstring( path.AsString().begin(), path.AsString().end() );
	ID3D11Resource* resource = nullptr;
	ID3D11ShaderResourceView* resourceView = nullptr;
	DirectX::CreateDDSTextureFromFile( m_device.GetDevice(), wPath.c_str(), &resource, &resourceView );

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	resourceView->GetDesc( &srvDesc );

	resourceView->Release();

	return { std::make_shared< renderer::TextureAsset >( path, std::make_unique< d3d11::D3D11Texture >( m_device, m_context, *static_cast< ID3D11Texture2D* >( resource ), srvDesc.Format ) ) };
}

static const char* c_handledExceptions[] = { "dds" };
forge::ArraySpan< const char* > d3d11::DDSTexturesLoader::GetHandledExtensions() const
{
	return c_handledExceptions;
}
