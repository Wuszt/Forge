#pragma once
#include "../Core/IAssetsLoader.h"

namespace forge
{
	class Asset;
}

namespace renderer
{
	struct MaterialData;

	class TinyObjModelsLoader : public forge::IAssetsLoader
	{		
	public:
		TinyObjModelsLoader( renderer::IRenderer& renderer );
		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const std::string& path ) const override;
		virtual forge::ArraySpan<const char *> GetHandledExtensions() const override;

	private:
		IRenderer& m_renderer;
	};
}

