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
		TinyObjModelsLoader( renderer::Renderer& renderer );
		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const forge::Path& path ) const override;
		virtual forge::ArraySpan<const char *> GetHandledExtensions() const override;

	private:
		Renderer& m_renderer;
	};
}

