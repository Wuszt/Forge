#pragma once
#include "../Core/IAssetsLoader.h"

namespace d3d11
{
	class DDSTexturesLoader : public forge::IAssetsLoader
	{
	public:
		DDSTexturesLoader( const D3D11Device& device, const D3D11RenderContext& context );

		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const forge::Path& path ) const override;
		virtual forge::ArraySpan<const char*> GetHandledExtensions() const override;

	private:
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
	};
}

