#pragma once
#include "IModelsLoader.h"

namespace renderer
{
	struct MaterialData;

	class TinyObjModelsLoader : public IModelsLoader
	{
	public:
		using IModelsLoader::IModelsLoader;
		virtual std::shared_ptr< renderer::Model > LoadModel( const std::string& path, std::vector< MaterialData >* materialsData = nullptr ) override;
	};
}

