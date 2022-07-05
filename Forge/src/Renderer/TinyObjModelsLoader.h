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
		virtual std::unique_ptr< forge::IAsset > LoadAsset( const std::string& path ) const override;
		virtual forge::ArraySpan<const char *> GetHandledExtensions() const override;

	private:
		IRenderer& m_renderer;
	};
}

