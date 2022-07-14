#pragma once
#include "../Core/IAssetsLoader.h"

namespace d3d11
{
	class D3D11Device;
	class D3D11RenderContext;

	class D3D11TexturesLoader : public forge::IAssetsLoader
	{
	public:
		D3D11TexturesLoader( const D3D11Device& device, const D3D11RenderContext& context );

		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const std::string& path ) const override;
		virtual forge::ArraySpan<const char *> GetHandledExtensions() const override;

	private:
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
	};
}

