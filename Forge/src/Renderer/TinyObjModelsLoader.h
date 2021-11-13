#pragma once
#include "IModelsLoader.h"

namespace renderer
{
	class TinyObjModelsLoader : public IModelsLoader
	{
	public:
		using IModelsLoader::IModelsLoader;
		virtual std::shared_ptr< renderer::Model > LoadModel( const std::string& path, std::vector< std::unique_ptr< renderer::ConstantBuffer > >* materialsData = nullptr ) override;
	};
}

